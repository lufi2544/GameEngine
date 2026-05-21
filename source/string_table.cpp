

global_f void
StringTableInit(string_table_t *table)
{
	arena_t *memory = EngineRequestMemory(enum_memory_sandbox_string_table);
	table->memory = memory;
	table->string_map = HASH_MAP(memory, 100, u32, string_t, hash_function_u32);
	
}


internal_f u32
StringTableKey(const char* string, u32 size)
{
	u8* content = (u8*)string;	
	u32 key = 0;
	
	for(u32 idx = 0; idx < size; ++idx)
	{
		u8 byte = content[idx];
		key += byte;
	}
	
	return key;
}

global_f string_t* 
StringTableAddString(string_table_t *table, const char* string)
{	
	string_t* result;
	string_t to_add = STRING_V(table->memory, string);
	u32 size = cstr_size(string);
	u32 string_hash = StringTableKey(string, size);
	
	HASH_MAP_ADD_GET(table->string_map, u32, string_t, string_hash, to_add, result);
	
	printf(" ========= \n");
	printf("String Table: Added string to the table. String --> \"%s\" \n", string);
	printf(" ========= \n");
	
	return result;
}

global_f string_t*
StringTableRequestString(string_table_t* table, const char* string)
{	
	string_t* result = 0;
	u32 size = cstr_size(string);
	u32 string_hash = StringTableKey(string, size);
	HASH_MAP_FIND(table->string_map, u32, string_t, string_hash, result);
	
	// Create a new entry if the string not found
	if(result == 0)
	{
		printf(" ========= \n");
		printf("String Table: string \"%s\" not found on the table. Adding it...\n", string);
		return StringTableAddString(table, string);
	}
	
	return result;
}