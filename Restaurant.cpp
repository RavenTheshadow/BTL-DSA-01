#include "main.h"

extern int MAXSIZE;

class imp_res : public Restaurant
{
	public:
		class Queue{
			public:
				class Node{
					public:
						customer *CUS_PTR;
						Node *next;

						Node(customer *CUS_PTR, Node *next = NULL) : CUS_PTR(CUS_PTR), next(next){}
						~Node(){
							customer *PRE = CUS_PTR->prev;
							customer *NEXT = CUS_PTR->next;

							PRE->next = NEXT;
							NEXT->prev = PRE;

							CUS_PTR->prev = NULL;
							CUS_PTR->next = NULL;

							delete CUS_PTR;

							next = NULL;
						}
				};
			
			public:
				Node *front;
				Node *back;

				Queue() : front(NULL), back(NULL){}

				void Enqueue(customer *CUS_PTR){
					if(!front){
						front = back = new Node(CUS_PTR);
					}
					else{
						back->next = new Node(CUS_PTR);
						back = back->next;
					}
				}

				void Dequeue(){
					if(!front) return;
					else{
						Node *Cur = front;
						front = front->next;

						if(!front) front = back = NULL;

						delete Cur;
					}
				}
				bool Is_Empty(){
					return (!front);
				}
				void clear(){
					while(front){
						Node *Cur = front;
						front = front->next;
						delete Cur;
					}
					front = NULL;
					back = NULL;
				}
				~Queue(){
					this->clear();
				}
		};


	public:
		customer *X;
		customer *CUS_IN_WAIT;
		customer *CUS_IN_WAIT_TAIL;

		Queue CUS_LST;

		int number_of_customer;
		int number_in_wait;
	private:
		bool Is_Only(customer *, string, int);
		bool Is_SameType(customer *A, customer *B){
			return (A->energy > 0 && B->energy > 0) || (A->energy < 0 && B->energy < 0);
		}
        customer *FIND_X(customer *LIST, int New_Energy, int number_element){
        	customer *RES = LIST;
			for(int i = 0, MAX_VAL = -abs(LIST->energy) + abs(New_Energy); i < number_element; i++){
			if(-abs(LIST->energy) + abs(New_Energy) > MAX_VAL){
				MAX_VAL = -abs(LIST->energy) + abs(New_Energy);
				RES = LIST;
			}
			LIST = LIST->next;
			}
			return RES;
		}

		void Reverse(customer *A, customer *B){
			// Reverse ==> THAY X = B

			//customer *A_Prev = B;
			// B <-> A
			customer *A_Next = A->next;
			customer *B_Prev = B->prev;
			if(A->prev == B){
				A->prev = B_Prev;
				B_Prev->next = A;  // Nối A và B-Prev

				B->next = A_Next;
				A_Next->prev = B; // Nối B và A->next

				B->prev = A;
				A->next = B;

			//	X = B;
			} 
			else{
				customer *A_Prev = A->prev;
				customer *B_Next = B->next;

				A->prev = B_Prev;
				A->next = B_Next;
				B_Prev->next = A;
				B_Next->prev = A;

				B->prev = A_Prev;
				B->next = A_Next;
				A_Prev->next = B;
				A_Next->prev = B;
			}
			X = B;
		}
    public:	
		imp_res() : X(NULL), CUS_IN_WAIT(NULL), number_of_customer(0), number_in_wait(0) {};

		void RED(string name, int energy)
		{
			//cout << name << " " << energy << endl;

			if(energy == 0 || number_in_wait + number_of_customer >= 2*MAXSIZE) return;

			if(number_of_customer == 0 && !X){
       			X = new customer(name, energy, NULL, NULL); // THÊM VÀO danh sách khách trong cửa hàng.
				X->prev = X;
				X->next = X;

				CUS_LST.Enqueue(X);

        		//CUS_LIS_TAIL = CUS_LIS = new customer(name, energy, NULL, NULL); // THÊM VÀO danh sách thứ tự khách hàng.
				
        		number_of_customer++;
        		return;
    		}

			// DUY NGÃ ĐỘC TÔN:
			
			if(Is_Only(X, name, number_of_customer) || Is_Only(CUS_IN_WAIT, name, number_in_wait)) return; 

			// Thêm khách vào nhà hàng hoặc danh sách đợi
			customer *New_Customer = new customer(name, energy, NULL, NULL);

			// Nếu cửa hàng đầy => thêm vào hàng đợi
			if(number_of_customer == MAXSIZE){
				if(CUS_IN_WAIT == NULL){
					CUS_IN_WAIT_TAIL = CUS_IN_WAIT = New_Customer;
				}
				else{
					// ADD to TAIL of CUS:
					CUS_IN_WAIT_TAIL->next = New_Customer;
					CUS_IN_WAIT_TAIL = New_Customer;
				}
				number_in_wait++;
				return;
			}
			if(number_of_customer < MAXSIZE/2){
				if(energy > X->energy){

					customer *Next_X = X->next;

					New_Customer->next = Next_X;
					Next_X->prev = New_Customer;

					New_Customer->prev = X;
					X->next = New_Customer;
				}
				else{
					customer *Pre_X = X->prev;

					New_Customer->prev = Pre_X;
                    Pre_X->next = New_Customer;

                    New_Customer->next = X;
					X->prev = New_Customer;
				}
				X = New_Customer;
				CUS_LST.Enqueue(New_Customer);
				number_of_customer++;
				return;
			}
			else{
				X = FIND_X(X, energy, number_of_customer);

				int DELTA_ENERGY =  New_Customer->energy - X->energy;

				if(DELTA_ENERGY > 0){
					customer *Next_X = X->next;

					New_Customer->next = Next_X;
					Next_X->prev = New_Customer;

					New_Customer->prev = X;
					X->next = New_Customer;
				}
				else{
					customer *Pre_X = X->prev;

					New_Customer->prev = Pre_X;
                    Pre_X->next = New_Customer;

                    New_Customer->next = X;
					X->prev = New_Customer;
				}
				X = New_Customer; // cập nhật lại X
				CUS_LST.Enqueue(New_Customer);
				number_of_customer++;
				return;
			}
		}

		void BLUE(int num)
		{
			num = (num > number_of_customer) ? number_of_customer : num;
			for(int i = 0; i < num; i++){

				int E = CUS_LST.front->CUS_PTR->energy;

				X = (E > 0) ? CUS_LST.front->CUS_PTR->next : CUS_LST.front->CUS_PTR->prev;
				// Cập nhật lại X theo đúng tinh thần.
				CUS_LST.Dequeue();
				number_of_customer--;
			}
			if(number_of_customer == 0) X = nullptr;

			while(CUS_IN_WAIT && number_in_wait > 0){
				customer *cur = CUS_IN_WAIT;

				CUS_IN_WAIT = CUS_IN_WAIT->next;

				number_in_wait--;

				if(!CUS_IN_WAIT) CUS_IN_WAIT_TAIL = CUS_IN_WAIT = NULL;

				this->RED(cur->name, cur->energy);

				cur->next = NULL;

				delete cur;
			}
		}
		void PURPLE()
		{
			cout << "purple"<< endl;
		}
		void REVERSAL()
		{
			//cout << "reversal" << endl;
			if(number_of_customer < 2) return;
			if(number_of_customer == 2) { if(Is_SameType(X, X->prev)) X = X->prev; return; } // Nếu number == 2 và 2 số cùng loại thì reverse 1 lần
			customer *Cur = X;
			int counter = 0;
			for(int i = 0; i < number_of_customer - 2; i++){
				if(Is_SameType(Cur, Cur->prev)){
					switch (counter)
					{
					case 0 :
						if(Cur->energy > 0){
							counter = 1;
						}
						else counter = -1;
						cout << "reversal: " <<Cur->energy<<" "<<Cur->prev->energy<<" "<<counter<< endl;
						Reverse(Cur, Cur->prev);

						break;
					case 1 :
						if(Cur->energy > 0){
							cout << "reversal: " <<Cur->energy<<" "<<Cur->prev->energy<<" "<<counter<< endl;
							Reverse(Cur, Cur->prev);
						}
						break;
					case 2 :
						if(Cur->energy < 0){
							cout << "reversal: " <<Cur->energy<<" "<<Cur->prev->energy<<" "<<counter<< endl;
							Reverse(Cur, Cur->prev);
						}
						break;
					default:
						break;
					}
				}
				Cur = Cur->prev;
			}
		}
		void UNLIMITED_VOID()
		{
			cout << "unlimited_void" << endl;
		}
		void DOMAIN_EXPANSION()
		{
			cout << "domain_expansion" << endl;
		}
		void LIGHT(int num)
		{
			if(!X) {
				cout<<"LIST_EMPTY\n";
				return;
			}
			customer *CUS;

			if(num == 0){
				CUS = CUS_IN_WAIT;
				for(int i = 0; i < number_in_wait; i++){
					CUS->print();
					CUS = CUS->next;
				}
				return;
			}
			CUS = X;
			if(num > 0){
				for(int i = 0; i < number_of_customer; i++){
					CUS->print();
					CUS = CUS->next;
				}

				cout<<"\n";
			}
			else{
				for(int i = 0; i < number_of_customer; i++){
					CUS->print();
					CUS = CUS->prev;
				}
			}
		}






		void printf_List(customer *List, int ne){
			for(int i = 0; i < ne; i++){
				cout<<List->name<<" "<<List->energy<<"\n";
				List = List->next;
			}
		}

		void printf_Queue(){
			cout<<"\n";
			for(auto i = CUS_LST.front; i != NULL; ){
				cout<<i->CUS_PTR->name<<" "<<i->CUS_PTR->energy<<"\n";
				i = i->next;
			}
		}

};

bool imp_res::Is_Only(customer *List, string name, int number_element){
	for(int i = 0; i < number_element; i++){
		if(List->name == name) return true;
		List = List->next;
	}
	return false;
}
