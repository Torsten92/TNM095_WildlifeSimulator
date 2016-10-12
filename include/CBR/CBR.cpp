#include "CBR.h"

Case::Case(float m_s, float m_h, int o_t, float o_si, float o_sp, float d, int res_a, float res_s, Object* res_int_o)
{
	my_size = m_s;
	my_hunger = m_h;

	other_type = o_t;
	other_size = o_si;
	other_speed = o_sp;

	resulting_action = res_a;
	resulting_speed = res_s;
	resulting_interacting_object = res_int_o;
}

CBR::CBR(Object* _me)
	: me_obj{_me}	
{}


int CBR::retrieve(vector<Object*> nearby_animals)
{
	auto me = dynamic_cast<Animal*>(me_obj); // needs to re-casted here because the type Animal can not be declared before CBR at creation because it uses it.
	
	vector<Case> similar_cases;

	//Grass eating is still hardcoded
	Object* interacting_object = me->get_interacting_object();

	for(auto o : nearby_animals)
	{
		//Check for possible grass to eat
		if(i_am_herbivore && o->get_type() == GRASS && dist(me->get_x(), me->get_y(), o->get_x(), o->get_y()) < 10.0) {
			interacting_object = o;
		}
		else if(Animal* a = dynamic_cast<Animal*>(o)) {
			if(!a->is_alive() && dist(me->get_x(), me->get_y(), o->get_x(), o->get_y()) < 10.0) {
				interacting_object = o;
			}
			
			if(a->get_type() == me->get_type()) continue; // should not exist, but skip cases with the same type
			for(auto& c : cases) {
				float w_type = c.other_type == a->get_type() ? 0.5 : 0.0;
				float w_speed = c.other_speed != -1 ? (1 - min(abs(c.other_speed - a->get_speed()) / 5.0, 1.0)) * 0.2 : 0.2;
				float w_dist = c.distance != -1 ? min(abs(c.distance - dist(me->get_x(), me->get_y(), a->get_x(), a->get_y())) / 500.0, 1.0) * 0.2 : 0.2;
				float w_size = c.other_size != -1 ? (1 - min(abs(c.other_size - a->get_size()) / 3.0, 1.0)) * 0.1 : 0.1;
				
				if(w_type + w_speed + w_dist + w_size > 0.4) {
					similar_cases.push_back(c);
				}
			}
		}
	}

	// if we found no similar cases, do the hardcoded options
	if(!similar_cases.size()) {
		if(me->get_hunger() > 0.8 && me->get_interacting_object() == nullptr) {
			return IDLE;
		}
		else if(interacting_object != nullptr) {
			me->set_interacting_object(interacting_object);
			return EAT;
		}
		else {
			return FIND_FOOD;
		}
	}
	else {
		return i_am_herbivore ? reason_herbivore(similar_cases) : reason_carnivore(similar_cases);
	}
}

// adapt the cases to either use a modified version of one of these cases, or make a completely new 
// case, adding it to the case-base, and then returning its solution.
int CBR::reason_herbivore(vector<Case> cases)
{
	return DEAD;
}

// adapt the cases to either use a modified version of one of these cases, or make a completely new 
// case, adding it to the case-base, and then returning its solution.
int CBR::reason_carnivore(vector<Case> cases)
{
	auto me = dynamic_cast<Animal*>(me_obj);
	Case* res = nullptr;
	for(auto& c : cases) {
		//cout << "reasoning about case: " << c.other_type << " - " << c.other_speed << ", " << c.other_size << endl;
	}

	return IDLE;
}

void CBR::adapt_case(Case c)
{

}

void CBR::add_case(Case c)
{
	cases.push_back(c);
}

void CBR::init(int type)
{
	//auto me = dynamic_cast<Animal*>(me_obj);
	switch(type)
	{
		case DEER:
		{
			i_am_herbivore = true;

			add_case(Case(1.0, -1, WOLF, 1.0, 1.0, 300, FLEE));
			break;
		}
		case WOLF:
		{
			add_case(Case(1.0, 0.5, DEER, 1.0, 1.0, -1, ATTACK));
			break;
		}
	}
}