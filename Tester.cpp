#include <iostream>
#include <string>

int iteration = 0;

#include "truth_table.h"
#include "truth_table.cpp"
#include "linked_list.cpp"
#include "ptr_linked_list.h"
#include "ptr_linked_list.cpp"
#include "log.cpp"

using namespace std;

void doThing() {

	log l(AND,3);
	log* p;
	log* q;

	p = new log("a");
	l.args.append(p);
	delete p;
	p = new log(NOT,3);
	q = new log("b");
	p->args.append(q);
	delete q;
	l.args.append(p);
	delete p;
	p = new log(OR,3);
	q = new log("c");
	p->args.append(q);
	delete q;
	q = new log("d");
	p->args.append(q);
	delete q;
	l.args.append(p);
	delete p;

	log r(OR,3);

	p = new log("b");
	r.args.append(p);
	delete p;
	p = new log(NOT,3);
	q = new log("a");
	p->args.append(q);
	delete q;
	r.args.append(p);
	delete p;
	p = new log(OR,3);
	q = new log("c");
	p->args.append(q);
	delete q;
	q = new log("d");
	p->args.append(q);
	delete q;
	q = p->invert();
	delete p;
	r.args.append(q);
	delete q;

	cout << l.to_string() << endl;
	cout << r.to_string() << endl;

	if (l.contains_same_elements(&r)) {
		cout << "L has same elements R" << endl;
	} else {
		cout << "L does not have same elements as R" << endl;
	}
	cout << l.to_string() << endl;
	cout << r.to_string() << endl;
	
	if (r.contains_same_elements(&l)) {
		cout << "R has same elements L" << endl;
	} else {
		cout << "R does not have same elements as L" << endl;
	}
	cout << l.to_string() << endl;
	cout << r.to_string() << endl;

	cout << "DONE" << endl;
}

int main() {
	doThing();
	cout << "FINISHED" << endl;
}