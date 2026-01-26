/* date = January 26th 2026 0:21 pm */


#include "sys/stat.h"


internal_f buffer_t
read_file(arena_t *_arena, const char *_file_name)
{
	buffer_t result;
	result.size = 0;
	result.data = 0;
	if(!_arena || !_file_name)
	{
		// TOD LOG
		return result;
	}
	
	FILE* file = fopen(_file_name, "rb");
	if(!file)
	{
		fprintf(stderr, "Unable to open the file %s \n", _file_name);
		return result;
	}
	
#ifdef _WINDOWS
	struct _stat64 _stat;
	s32 stat_result = _stat64(_file_name, &_stat);	
#endif // _WINDOWS
	
	
#ifdef _APPLE
	struct stat _stat;
	s32 stat_result = stat(_file_name, &_stat);
#endif // _APPLE
	
	if(stat_result != 0)
	{
		return result;
	}	
	
    s32 file_size = 0;
    
#ifdef _APPLE
    file_size = _stat.st_size;
#endif 
    
#ifdef _WINDOWS
    file_size = _stat.st_size;
#endif
    
	result = create_buffer(_arena, sizeof(u8) * file_size); // reduntand but well..
    
    
	u64 read_elements = fread(result.data, sizeof(u8), result.size, file);
	if(read_elements == 0)
	{
		// successfully read the file do stuff
		fprintf(stderr, "Unable to read the file %s \n", _file_name);		
		return result;
	}
	
	
	return result;
}
