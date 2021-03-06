#include <fstream>
#include <iostream>
#include <vector>
#include <queue> 
#include <map>
#include <string>
#include <thread>
#include <array>
#include <mutex>
#include <algorithm>

#include <boost/crc.hpp>

// using namespace std;

struct jaaX_Open_Addressing_
{
    std::string key;
    std::string value;
    int number_of_collision_elements;
    struct jaaX_Open_Addressing_ *next;
    struct jaaX_Open_Addressing_ *lastIndex;
};

int GetCrc32(const std::string& my_string) {
    boost::crc_32_type result;
    result.process_bytes(my_string.data(), my_string.length());
    return result.checksum();
}

class jaax_MultiMap {
	
	private:
		std::map<int, jaaX_Open_Addressing_> jaaX_Map;
		std::mutex mutex_x;

	public:

		jaax_MultiMap(): jaaX_Map() {

		}

		~jaax_MultiMap() {

		}

		void insert_Element(int key, struct jaaX_Open_Addressing_ value) {
			std::lock_guard<std::mutex> guard(mutex_x);
			jaaX_Map[key] = value;
		}

		void insert_and_check(std::string key, std::string value) {

			std::lock_guard<std::mutex> guard(mutex_x);
			int i_key = GetCrc32(key);
			struct jaaX_Open_Addressing_ element = {key, value, 0, NULL, NULL};
			std::map<int, jaaX_Open_Addressing_>::iterator it = jaaX_Map.find(i_key);
			if(it != jaaX_Map.end())
			{
			   // Add colision element found code here
				jaaX_Open_Addressing_ *node=new jaaX_Open_Addressing_;
				node->key = key;
				node->value = value;
				node->number_of_collision_elements = 1;
				node->next = NULL;
				node->lastIndex = NULL;

				if(jaaX_Map[i_key].number_of_collision_elements == 0){
				
					jaaX_Map[i_key].next = node;
					jaaX_Map[i_key].lastIndex = node;
					jaaX_Map[i_key].number_of_collision_elements = jaaX_Map[i_key].number_of_collision_elements + 1;
				
				} else {
				
					jaaX_Map[i_key].lastIndex->next = node;
					// jaaX_Map.find(i_key)->lastIndex->lastIndex = node; // Not needed
					jaaX_Map[i_key].lastIndex = node;
					jaaX_Map[i_key].number_of_collision_elements = jaaX_Map[i_key].number_of_collision_elements + 1;
				
				}

			} else {
				// Insert's element if collision does not occur
				insert_Element(i_key, element);	
			}
			
		}

		std::vector<std::string> get_Element(std::string key) {

			std::lock_guard<std::mutex> guard(mutex_x);
			int i_key = GetCrc32(key);
			std::map<int, jaaX_Open_Addressing_>::iterator it = jaaX_Map.find(i_key);
			if(it != jaaX_Map.end())
			{
			   // Currently just returns non colision element found add colision case code here.
				if(jaaX_Map[i_key].key != key) {

					jaaX_Open_Addressing_ *current=new jaaX_Open_Addressing_;
					current = jaaX_Map[i_key].next;
					while(current->key != key) {
				      current=current->next;	
				    }
				    return {current->key, current->value};
				} else {
					return {jaaX_Map[i_key].key, jaaX_Map[i_key].value};
				}
			} else {
				// Element's not found
				return {"", ""};
			}
		}

		void erase_element(std::string key) {

			std::lock_guard<std::mutex> guard(mutex_x);
			int i_key = GetCrc32(key);
			std::map<int, jaaX_Open_Addressing_>::iterator it = jaaX_Map.find(i_key);
			jaaX_Open_Addressing_ *current=new jaaX_Open_Addressing_;
			jaaX_Open_Addressing_ *previous=new jaaX_Open_Addressing_;			
			
			if(it != jaaX_Map.end()) {

				if(jaaX_Map[i_key].key != key) {

					current = jaaX_Map[i_key].next;
					while(current->key != key) {
					  previous = current;
				      current=current->next;	
				    }
				    if(current->next == NULL){ // Last item deleted
				    	previous->next = NULL;
				    	jaaX_Map[i_key].lastIndex = previous;
				    	delete current;
				    } else {
				    	previous->next = current->next;
				    	delete current;
				    }

				} else {
					if(jaaX_Map[i_key].number_of_collision_elements == 0) {

						jaaX_Map.erase(i_key);
					
					} else {

						current = jaaX_Map[i_key].next;
						jaaX_Map[i_key].key = current->key;
						jaaX_Map[i_key].value = current->value; 
						if(current->next == NULL){ // Only 2 item's present test
							jaaX_Map[i_key].lastIndex = NULL;
							delete current;
						} else {
							jaaX_Map[i_key].next = current->next;
							delete current;
						}

					}

				}
			} 
		}


		void put_element(std::string key, std::string value) {

			std::lock_guard<std::mutex> guard(mutex_x);
			int i_key = GetCrc32(key);
			
			if(jaaX_Map[i_key].key != key) {

				jaaX_Open_Addressing_ *current=new jaaX_Open_Addressing_;
				current = jaaX_Map[i_key].next;
				
				while(current->key != key) {
			    
			      current=current->next;	
			    
			    }
			    
			    current->value = value;

			} else {

				jaaX_Map[i_key].value = value;
			
			}
		}
};


int main() {
	jaax_MultiMap *jm = new jaax_MultiMap();
	jm->insert_and_check("Aalekh", "Nigam");
	std::cout <<  jm->get_Element("Aalekh")[1] << std::endl;
	jm->put_element("Aalekh", "Niigam");
	jm->erase_element("Aalekh");
	return 0;
}