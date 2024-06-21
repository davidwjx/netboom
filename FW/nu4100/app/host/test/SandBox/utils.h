#pragma once

#if 0

typedef HANDLE OS_LYRG_mutexT;

#if (defined _WIN32) || (defined _WIN64)
static int OS_LYRP_aquireMutex(OS_LYRG_mutexT *mutexP)
{
	int ret = SUCCESS_E;
	*mutexP = CreateMutex(NULL, FALSE, NULL);
	if (!*mutexP)
	{
		ret = FAIL_E;
	}
	return ret;
}

static int OS_LYRP_lockMutex(OS_LYRG_mutexT *mutexP)
{
	return  (WaitForSingleObject(*mutexP, INFINITE) == WAIT_OBJECT_0) ? SUCCESS_E : FAIL_E;
}

static int OS_LYRP_unlockMutex(OS_LYRG_mutexT *mutexP)
{
	int retVal = ReleaseMutex(*mutexP);
	if (retVal)
	{
		return SUCCESS_E;
	}
	return FAIL_E;
}

static int OS_LYRP_releaseMutex(OS_LYRG_mutexT *mutexP)
{
	int retVal = CloseHandle(*mutexP);
	if (retVal)
	{
		return SUCCESS_E;
	}
	return FAIL_E;
}
#endif

#endif