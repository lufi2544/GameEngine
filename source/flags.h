

global_f
bool HasFlag(u32 s, u32 flag)
{
	return s & flag;
}


global_f 
void SetFlag(u32 *flags, u32 to_set, bool value)
{
	if(value)
	{
		(*flags) |= to_set;
	}
	else
	{
		(*flags) &= (~to_set);
	}
}

