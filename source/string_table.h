/* date = May 16th 2026 9:35 am */


//////////////////////////////////////////////
// String look up for persistent strings memory
//////////////////////////////////////////////

struct string_table_t
{
	arena_t *memory;
	
	hash_map_t string_map;
};


global_f void
StringTableInit(string_table_t *table);

global_f string_t* 
StringTableAddString(string_table_t *table, char* string);



global_f string_t*
StringTableRequestString(string_table_t* table, char* string);
