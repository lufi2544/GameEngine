


global_f void
StringTableInit(string_table_t *table)
{
	arena_t *memory = EngineRequestMemory(enum_memory_sandbox_string_table);
	table->memory = memory;
	table->string_map = HASH_MAP(memory, 100, u32, string_t, hash_function_u32);
	
}


global_f string_t* 
StringTableAddString(string_table_t *table, char* string)
{	
	string_t* result;
	string_t to_add = STRING_V(table->memory, string);
	u32 string_hash = hash_function_string(&to_add, sizeof(string_t));
	
	HASH_MAP_ADD_GET(table->string_map, u32, string_t, string_hash, to_add, result);
	
	return result;
}



global_f string_t*
StringTableRequestString(string_table_t* table, char* string)
{	
	string_t* result = 0;
	u32 string_hash = hash_function_c_string(string, sizeof(string_t));	
	HASH_MAP_FIND(table->string_map, u32, string_t, string_hash, result);
	
	return result;
}