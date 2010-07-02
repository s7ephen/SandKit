//Converted to dll and .h by stephen@sa7ori.org. 
//Dec 2009
#undef UNICODE //problems with assertions if this doesnt exist.

// ******** EXPORT PROTOTYPES **********

BOOL _DumpToken(HANDLE Token);

// *************************************

BOOL _DumpToken(HANDLE Token) {
/*
 * Code to dump out a token on Windows
 * January 2003
 *
 * Matt Conover (shok@dataforce.net)
 * http://www.w00w00.org
 */
	unsigned int i;
	DWORD Size, UserSize, DomainSize;
	SID *sid;
	SID_NAME_USE SidType;
	char UserName[64], DomainName[64], PrivilegeName[64];

	DWORD SessionID;
	TOKEN_TYPE Type;
	TOKEN_STATISTICS *Statistics;
	TOKEN_SOURCE Source;
	TOKEN_OWNER *Owner;
	TOKEN_USER *User;
	TOKEN_PRIMARY_GROUP *PrimaryGroup;
	TOKEN_DEFAULT_DACL *DefaultDacl;
	TOKEN_PRIVILEGES *Privileges;
	TOKEN_GROUPS *Groups;
	SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;

	DWORD EntryCount;
	EXPLICIT_ACCESS *ExplicitEntries, *ExplicitEntry;

	memset(PrivilegeName, 0, sizeof(PrivilegeName));
	memset(UserName, 0, sizeof(UserName));
	memset(DomainName, 0, sizeof(DomainName));

	printf("This is a %s token\n", IsTokenRestricted(Token) ? "restricted" : "unrestricted");

	/////////////////////////////////////////////////////////////////
	// Dump token type

	Size = 0;
	GetTokenInformation(Token, TokenType, &Type, sizeof(TOKEN_TYPE), &Size);
	if (!Size)
	{
		printf("Error getting token type: error code 0x%lx\n", GetLastError());
		return FALSE;
	}

	printf("Token type: ");
	if (Type == TokenPrimary) printf("primary\n");
	else printf("impersonation\n");

	if (Type == TokenImpersonation)
	{
		Size = 0;
		if (!GetTokenInformation(Token, TokenImpersonationLevel, &ImpersonationLevel, sizeof(SECURITY_IMPERSONATION_LEVEL), &Size) || !Size)
		{
			printf("Error getting impersonation level: error code 0x%lx\n", GetLastError());
			return FALSE;
		}

		printf("Impersonation level: ");
		switch (ImpersonationLevel)
		{
			case SecurityAnonymous:
				printf("anonymous\n");
				break;
			case SecurityIdentification:
				printf("identification\n");
				break;
			case SecurityImpersonation:
				printf("impersonation\n");
				break;
			case SecurityDelegation:
				printf("delegation\n");
				break;
		}
	}

	/////////////////////////////////////////////////////////////////
	// Dump the token IDs

	// Get the Token and Authentication IDs
	Size = 0;
	GetTokenInformation(Token, TokenStatistics, NULL, 0, &Size);
	if (!Size)
	{
		printf("Error getting token statistics: error code 0x%lx\n", GetLastError());
		return FALSE;
	}
	
	assert((Statistics = (TOKEN_STATISTICS *)malloc(Size)));
	assert(GetTokenInformation(Token, TokenStatistics, Statistics, Size, &Size) && Size);
	printf("Token ID: 0x%lx\n", Statistics->TokenId.LowPart);
	printf("Authentication ID: 0x%lx\n", Statistics->AuthenticationId.LowPart);
	free(Statistics);

	// Get the Session ID
	Size = 0;
	if (!GetTokenInformation(Token, TokenSessionId, &SessionID, sizeof(SessionID), &Size) || !Size)
	{
		printf("Error getting the Session ID: error code 0x%lx\n", GetLastError());
		return FALSE;
	}
	if (SessionID) printf("Session ID = 0x%lx\n", SessionID);

	/////////////////////////////////////////////////////////////////
	// Dump token owner

	Size = 0;
	GetTokenInformation(Token, TokenOwner, NULL, 0, &Size);
	if (!Size)
	{
		printf("Error getting token owner: error code0x%lx\n", GetLastError());
		return FALSE;
	}
	
	assert((Owner = (TOKEN_OWNER *)malloc(Size)));
	assert(GetTokenInformation(Token, TokenOwner, Owner, Size, &Size) && Size);
	assert((Size = GetLengthSid(Owner->Owner)));
	assert((sid = (SID *)malloc(Size)));
	assert(CopySid(Size, sid, Owner->Owner));
	UserSize = sizeof(UserName)-1;
	DomainSize = sizeof(DomainName)-1;
	assert(LookupAccountSid(NULL, sid, UserName, &UserSize, DomainName, &DomainSize, &SidType));
	free(sid);

	printf("Token's owner: %s\\%s ", DomainName, UserName);
	switch (SidType)
	{
		case SidTypeUser:
			printf("(user)\n");
			break;
		case SidTypeGroup:
			printf("(group)\n");
			break;
		case SidTypeDomain:
			printf("(domain)\n");
			break;
		case SidTypeAlias:
			printf("(alias)\n");
			break;
		case SidTypeWellKnownGroup:
			printf("(well-known group)\n");
			break;
		case SidTypeDeletedAccount:
			printf("(deleted account)\n");
			break;
		case SidTypeInvalid:
			printf("(invalid)\n");
			break;
		case SidTypeUnknown:
			printf("(unknown)\n");
			break;
		case SidTypeComputer:
			printf("(computer)\n");
			break;
	}

	free(Owner);

	/////////////////////////////////////////////////////////////////
	// Dump token source

	Size = 0;
	if (!GetTokenInformation(Token, TokenSource, &Source, sizeof(TOKEN_SOURCE), &Size) || !Size)
	{
		printf("Error getting token source: error code 0x%lx\n", GetLastError());
		return FALSE;
	}

	printf("Token's source: ");
	for (i = 0; i < 8 && Source.SourceName[i]; i++) printf("%c", Source.SourceName[i]);
	printf(" (0x%lx)\n", Source.SourceIdentifier.LowPart);

	/////////////////////////////////////////////////////////////////
	// Dump token user

	Size = 0;
	GetTokenInformation(Token, TokenUser, NULL, 0, &Size);
	if (!Size)
	{
		printf("Error getting token user: error code 0x%lx\n", GetLastError());
		return FALSE;
	}

	assert((User = (TOKEN_USER *)malloc(Size)));
	assert(GetTokenInformation(Token, TokenUser, User, Size, &Size) && Size);
	assert((Size = GetLengthSid(User->User.Sid)));
	assert((sid = (SID *)malloc(Size)));
	assert(CopySid(Size, sid, User->User.Sid));
	UserSize = sizeof(UserName)-1;
	DomainSize = sizeof(DomainName)-1;
	assert(LookupAccountSid(NULL, sid, UserName, &UserSize, DomainName, &DomainSize, &SidType));
	free(sid);

	printf("Token's user: %s\\%s ", DomainName, UserName);
	switch (SidType)
	{
		case SidTypeUser:
			printf("(user)\n");
			break;
		case SidTypeGroup:
			printf("(group)\n");
			break;
		case SidTypeDomain:
			printf("(domain)\n");
			break;
		case SidTypeAlias:
			printf("(alias)\n");
			break;
		case SidTypeWellKnownGroup:
			printf("(well-known group)\n");
			break;
		case SidTypeDeletedAccount:
			printf("(deleted account)\n");
			break;
		case SidTypeInvalid:
			printf("(invalid)\n");
			break;
		case SidTypeUnknown:
			printf("(unknown)\n");
			break;
		case SidTypeComputer:
			printf("(computer)\n");
			break;
	}

	free(User);

	/////////////////////////////////////////////////////////////////
	// Primary group

	Size = 0;
	GetTokenInformation(Token, TokenPrimaryGroup, NULL, 0, &Size);
	if (!Size)
	{
		printf("Error getting primary group: error code 0x%lx\n", GetLastError());
		return FALSE;
	}

	assert((PrimaryGroup = (TOKEN_PRIMARY_GROUP *)malloc(Size)));
	assert(GetTokenInformation(Token, TokenPrimaryGroup, PrimaryGroup, Size, &Size) && Size);

	assert((Size = GetLengthSid(PrimaryGroup->PrimaryGroup)));
	assert((sid = (SID *)malloc(Size)));
	assert(CopySid(Size, sid, PrimaryGroup->PrimaryGroup));
	UserSize = sizeof(UserName)-1;
	DomainSize = sizeof(DomainName)-1;
	assert(LookupAccountSid(NULL, sid, UserName, &UserSize, DomainName, &DomainSize, &SidType));
	free(sid);

	printf("Token's primary group: %s\\%s ", DomainName, UserName);
	switch (SidType)
	{
		case SidTypeUser:
			printf("(user)\n");
			break;
		case SidTypeGroup:
			printf("(group)\n");
			break;
		case SidTypeDomain:
			printf("(domain)\n");
			break;
		case SidTypeAlias:
			printf("(alias)\n");
			break;
		case SidTypeWellKnownGroup:
			printf("(well-known group)\n");
			break;
		case SidTypeDeletedAccount:
			printf("(deleted account)\n");
			break;
		case SidTypeInvalid:
			printf("(invalid)\n");
			break;
		case SidTypeUnknown:
			printf("(unknown)\n");
			break;
		case SidTypeComputer:
			printf("(computer)\n");
			break;
	}
	
	free(PrimaryGroup);

	/////////////////////////////////////////////////////////////////
	// Dump default dacl
	
	Size = 0;
	GetTokenInformation(Token, TokenDefaultDacl, NULL, 0, &Size);
	if (!Size)
	{
		printf("Error getting default DACL: error code 0x%lx\n", GetLastError());
		return FALSE;
	}

	assert((DefaultDacl = (TOKEN_DEFAULT_DACL *)malloc(Size)));
	assert(GetTokenInformation(Token, TokenDefaultDacl, DefaultDacl, Size, &Size) && Size);
	printf("Default DACL (%d bytes):\n", DefaultDacl->DefaultDacl->AclSize);
	printf("ACE count: %d\n", DefaultDacl->DefaultDacl->AceCount);
		
	if (GetExplicitEntriesFromAcl(DefaultDacl->DefaultDacl, &EntryCount, &ExplicitEntries) != ERROR_SUCCESS)
	{
		printf("GetExplicitEntriesFromAcl failed: error code 0x%lx\n", GetLastError());
		return FALSE;
	}

	for (i = 0, ExplicitEntry = ExplicitEntries; i < EntryCount; i++, ExplicitEntry++)
	{
		printf("ACE %d:\n", i);
		
		printf("  Applies to: ");
		if (ExplicitEntry->Trustee.TrusteeForm == TRUSTEE_BAD_FORM) printf("trustee is in bad form\n");
		else if (ExplicitEntry->Trustee.TrusteeForm == TRUSTEE_IS_NAME) printf("%s ", ExplicitEntry->Trustee.ptstrName);
		else if (ExplicitEntry->Trustee.TrusteeForm == TRUSTEE_IS_SID)
		{
			assert((Size = GetLengthSid((SID *)ExplicitEntry->Trustee.ptstrName)));
			assert((sid = (SID *)malloc(Size)));
			assert(CopySid(Size, sid, (SID *)ExplicitEntry->Trustee.ptstrName));
			UserSize = sizeof(UserName)-1;
			DomainSize = sizeof(DomainName)-1;
			assert(LookupAccountSid(NULL, sid, UserName, &UserSize, DomainName, &DomainSize, &SidType));
			free(sid);

			printf("%s\\%s ", DomainName, UserName);
		}
		else
		{
			printf("Unhandled trustee form %d\n", ExplicitEntry->Trustee.TrusteeForm);
			return FALSE;
		}

		switch (ExplicitEntry->Trustee.TrusteeType)
		{
			case TRUSTEE_IS_USER:
				printf("(user)\n");
				break;
			case TRUSTEE_IS_GROUP:
				printf("(group)\n");
				break;
			case TRUSTEE_IS_DOMAIN:
				printf("(domain)\n");
				break;
			case TRUSTEE_IS_ALIAS:
				printf("(alias)\n");
				break;
			case TRUSTEE_IS_WELL_KNOWN_GROUP:
				printf("(well-known group)\n");
				break;
			case TRUSTEE_IS_DELETED:
				printf("(deleted)\n");
				break;
			case TRUSTEE_IS_INVALID:
				printf("(invalid)\n");
				break;
			case TRUSTEE_IS_UNKNOWN:
				printf("(unknown)\n");
				break;
		}

		printf("  ACE inherited by: ");
		if (!ExplicitEntry->grfInheritance) printf("not inheritable");
		if (ExplicitEntry->grfInheritance & CONTAINER_INHERIT_ACE) printf("[containers] ");
		if (ExplicitEntry->grfInheritance & INHERIT_ONLY_ACE) printf("[inherited objects]");
		if (ExplicitEntry->grfInheritance & NO_PROPAGATE_INHERIT_ACE) printf("[inheritance flags not propagated] ");
		if (ExplicitEntry->grfInheritance & OBJECT_INHERIT_ACE) printf("[objects] ");
		if (ExplicitEntry->grfInheritance & SUB_CONTAINERS_AND_OBJECTS_INHERIT) printf("[containers and objects] ");
		if (ExplicitEntry->grfInheritance & SUB_CONTAINERS_ONLY_INHERIT) printf("[sub-containers] ");
		if (ExplicitEntry->grfInheritance & SUB_OBJECTS_ONLY_INHERIT) printf("[sub-objects] ");
		printf("\n");
		
		printf("  Access permission mask = 0x%08lx\n", ExplicitEntry->grfAccessPermissions);
		printf("  Access mode: ");
		switch (ExplicitEntry->grfAccessMode)
		{
			case GRANT_ACCESS:
				printf("grant access\n");
				break;
			case SET_ACCESS:
				printf("set access (discards any previous controls)\n");
				break;
			case DENY_ACCESS:
				printf("deny access\n");
				break;
			case REVOKE_ACCESS:
				printf("revoke access (discards any previous controls)\n");
				break;
			case SET_AUDIT_SUCCESS:
				printf("generate success audit event\n");
				break;
			case SET_AUDIT_FAILURE:
				printf("generate failure audit event\n");
				break;
		}
	}

	LocalFree(ExplicitEntries);
	free(DefaultDacl);

	/////////////////////////////////////////////////////////////////
	// Dump privileges

	Size = 0;
	GetTokenInformation(Token, TokenPrivileges, NULL, 0, &Size);
	if (!Size)
	{
		printf("Error getting token privileges: error code 0x%lx\n", GetLastError());
		return FALSE;
	}

	assert((Privileges = (TOKEN_PRIVILEGES *)malloc(Size)));
	assert(GetTokenInformation(Token, TokenPrivileges, Privileges, Size, &Size) && Size);

	if (Privileges->PrivilegeCount) printf("Token's privileges (%d total):\n", Privileges->PrivilegeCount);
	for (i = 0; i < Privileges->PrivilegeCount; i++)
	{
		Size = sizeof(PrivilegeName)-1;
		assert(LookupPrivilegeName(NULL, &Privileges->Privileges[i].Luid, PrivilegeName, &Size));

		printf("  %s (0x%lx) = ", PrivilegeName, Privileges->Privileges[i].Luid.LowPart);
		if (!Privileges->Privileges[i].Attributes) printf("disabled");
		if (Privileges->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED)
		{
			if (Privileges->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED_BY_DEFAULT) printf("[enabled by default] ");
			else printf("[enabled] ");
		}
		if (Privileges->Privileges[i].Attributes & SE_PRIVILEGE_USED_FOR_ACCESS) printf("used for access] ");
		printf("\n");
	}
	
	free(Privileges);

	/////////////////////////////////////////////////////////////////
	// Dump restricted SIDs

	Size = 0;
	GetTokenInformation(Token, TokenRestrictedSids, NULL, 0, &Size);
	if (!Size)
	{
		printf("Error getting token restricted SIDs: error code 0x%lx\n", GetLastError());
		return FALSE;
	}

	assert((Groups = (TOKEN_GROUPS *)malloc(Size)));
	assert(GetTokenInformation(Token, TokenRestrictedSids, Groups, Size, &Size) && Size);

	if (Groups->GroupCount) printf("Restricted SIDs (%d total):\n", Groups->GroupCount);
	for (i = 0; i < Groups->GroupCount; i++)
	{
		assert((Size = GetLengthSid(Groups->Groups[i].Sid)));
		assert((sid = (SID *)malloc(Size)));
		assert(CopySid(Size, sid, Groups->Groups[i].Sid));
		UserSize = sizeof(UserName)-1;
		DomainSize = sizeof(DomainName)-1;
		assert(LookupAccountSid(NULL, sid, UserName, &UserSize, DomainName, &DomainSize, &SidType));
		free(sid);

		printf("  [%d] %s\\%s (", i, DomainName, UserName);
		switch (SidType)
		{
			case SidTypeUser:
				printf("user)\n");
				break;
			case SidTypeGroup:
				printf("group)\n");
				break;
			case SidTypeDomain:
				printf("domain)\n");
				break;
			case SidTypeAlias:
				printf("alias)\n");
				break;
			case SidTypeWellKnownGroup:
				printf("well-known group)\n");
				break;
			case SidTypeDeletedAccount:
				printf("deleted account)\n");
				break;
			case SidTypeInvalid:
				printf("invalid)\n");
				break;
			case SidTypeUnknown:
				printf("unknown)\n");
				break;
			case SidTypeComputer:
				printf("computer)\n");
				break;
		}

		printf("  [%d] Group is: ", i);
		if (!Groups->Groups[i].Attributes) printf("disabled\n");
		if (Groups->Groups[i].Attributes & SE_GROUP_ENABLED)
		{
			if (Groups->Groups[i].Attributes & SE_GROUP_ENABLED_BY_DEFAULT) printf("[enabled by default] ");
			else printf("[enabled] ");
		}
		if (Groups->Groups[i].Attributes & SE_GROUP_LOGON_ID) printf("[logon_id] ");
		if (Groups->Groups[i].Attributes & SE_GROUP_MANDATORY) printf("[mandatory] ");
		if (Groups->Groups[i].Attributes & SE_GROUP_USE_FOR_DENY_ONLY) printf("[used for deny only] ");
		printf("\n");
	}
	free(Groups);

	return TRUE;
}
