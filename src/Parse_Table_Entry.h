/*
 * Parse_Table_Entry.h
 *
 *  Created on: Jun 5, 2011
 *      Author: ahmedkotb
 */

#ifndef PARSE_TABLE_ENTRY_H_
#define PARSE_TABLE_ENTRY_H_

class Parse_Table_Entry {
private:
	bool epsilon;
	bool sync;
	int production_num;

public:
	Parse_Table_Entry();

	bool has_entry();

	void set_epsilon();

	//getters
	bool is_sync();
	int get_production_num();
	bool is_epsilon();
	//setters
	void set_sync(bool);
	void set_production_num(int);
	void set_epsilon(bool);

	virtual ~Parse_Table_Entry();
};

#endif /* PARSE_TABLE_ENTRY_H_ */
