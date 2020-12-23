#include "q_shared.h"
#include "xzone.h"
#include "qcommon.h"
#include "ui_shared.h"

/********************************************************

	Generic Procedures for dumping XAssets

*********************************************************/
typedef struct{
	XAssetType type;
	qboolean required;
	XAssetHeader header; //not for storing in file but used by loader
	int reserved;
	char* name;
}XAssetDependencyEntryHeader_t;

typedef struct XAssetFileHeader_s
{
	char magic[9]; //"IW3XASSET"
	long headerlen; //Length of header including magic
	XAssetType type;
	long datastart; //Datastart is also alloc start
	long datalen;	  //Len from datastart
	int reserved[20];
	int numdependencies;
	XAssetDependencyEntryHeader_t dependencies[1]; //Variable length
}XAssetFileHeader_t;

typedef struct
{
	XAssetDependencyEntryHeader_t *dependencylist;
	int size;
}dependencyStack_t;

#define DEPENDENCYSTACKSIZE 16

static jmp_buf error_dropoff;
static msg_t imagestream;
static XAssetDependencyEntryHeader_t *dependencies;
static dependencyStack_t dependenciesStack[DEPENDENCYSTACKSIZE]; //Allow depth of 16 entries
static int dependenciesStackPointer;
static int dependenciescounter;
static int maxdependencies;

void XAssetLoad_Error(char* errormsg)
{
	Com_Printf(CON_FIRST_DEBUG_CHANNEL, "Error loading XAsset: %s\n", errormsg);
	longjmp(error_dropoff, 1);
}

#define XASSETLOADPOINTER(p) XAssetLoadPointer((void**)p, sizeof(**p))
#define XASSETLOADZEROPOINTER(p) XAssetLoadZeroPointer((void**)p, sizeof(**p))
#define XASSETSTOREPOINTER(p) XAssetStorePointer((void**)p, sizeof(**p))


menuDef_t* MenuLoadMenuDef_t();
void MenuDumpMenuDef_t(XAssetHeader header);
MenuList* Load_MenuList( );
void Dump_MenuList(XAssetHeader header);
void Dump_XAnimParts(XAssetHeader header);


void XAssetLoadPointer(void** ptr, size_t s)
{
	unsigned int offset = *(unsigned int*)ptr;

	if(*(unsigned int*)ptr == 0)
	{
		//Nothing to load (Zero pointer)
		return;
	}
	if(offset >= imagestream.maxsize)
	{
			XAssetLoad_Error("Out of range pointer");
	}
	if(offset + s > imagestream.maxsize)
	{
			XAssetLoad_Error("Out of range object");
	}
	(*(unsigned int*)ptr) += (unsigned int)imagestream.data;
}

void XAssetLoadZeroPointer(void** ptr, size_t s)
{
	unsigned int offset = *(unsigned int*)ptr;
	if(offset >= imagestream.maxsize)
	{
			XAssetLoad_Error("Out of range zeropointer");
	}
	if(offset + s > imagestream.maxsize)
	{
			XAssetLoad_Error("Out of range object");
	}
	(*(unsigned int*)ptr) += (unsigned int)imagestream.data;
}

void XAssetLoadString(char** pString)
{
	if(*pString == NULL)
	{
		return;
	}
	XAssetLoadPointer((void**)pString, 0);
	char* string = *pString;
	if((byte*)(string + strlen(string)) >= imagestream.data + imagestream.maxsize)
	{
		XAssetLoad_Error("Out of range string");
	}
}

void* XAssetStorePointer(void** offset, size_t s)
{
	if(*(unsigned int*)offset == 0)
	{
		return NULL;
	}
	unsigned int startoffset = imagestream.cursize;
	MSG_WriteData(&imagestream, *offset, s);
	(*((unsigned int*)offset)) = startoffset;
	return startoffset + imagestream.data;
}

void XAssetStoreString(const char** offset)
{
	if(*offset == NULL)
	{
		return;
	}
	XAssetStorePointer((void**)offset, strlen(*offset) +1);
}

int XAssetAddDependency(XAssetType type, const char* name, qboolean required)
{
	int i;
	if(dependencies == NULL)
	{
		Com_Error(ERR_DROP, "XAssetAddDependency: Not initialized");
	}
	if(maxdependencies == dependenciescounter)
	{
		Com_Error(ERR_DROP, "XAssetAddDependency: maxdependencies == dependenciescounter");
	}
	//First see if we can find this entry already in our table
	for(i = 0; i < dependenciescounter; ++i)
	{
		if(dependencies[i].type == type && Q_stricmp(dependencies[i].name, name) == 0)
		{
			if(required)
			{
				dependencies[dependenciescounter].required = qtrue;
			}
			return i +1;
		}
	}
	//Create a new entry
	memset(&dependencies[dependenciescounter], 0, sizeof(dependencies[dependenciescounter]));
	dependencies[dependenciescounter].type = type;
	dependencies[dependenciescounter].required = required;
	dependencies[dependenciescounter].name = strdup(name);
	++dependenciescounter;
	return dependenciescounter;
}

void XAssetFreeDependencyList() //For dumper only. Not for loader
{
	int i;

	if(dependencies == NULL)
	{
		return;
	}
	for(i = 0; i < dependenciescounter; ++i)
	{
		free(dependencies[i].name);
	}
}

XAssetHeader XAssetGetDependencyAsset(int listindex)
{
	if(listindex == 0)
	{
		return (XAssetHeader)NULL;
	}
	if(dependencies == NULL)
	{
		Com_Error(ERR_DROP, "XAssetGetDependencyAsset: Not initialized");
	}

	--listindex; //Index starts with 1

	if(listindex >= dependenciescounter)
	{
		Com_PrintError(CON_FIRST_DEBUG_CHANNEL, "DependencyEntry %d is invalid\n", listindex);
		return (XAssetHeader)NULL;
	}
	return dependencies[listindex].header;
}

void XAssetDependencyStackPop()
{
	--dependenciesStackPointer;
	if(dependenciesStackPointer < 0)
	{
		Com_Error(ERR_FATAL, "XAssetDependencyStackPop reached -1");
	}
	dependencies = dependenciesStack[dependenciesStackPointer].dependencylist;
	dependenciescounter = dependenciesStack[dependenciesStackPointer].size;
}

void XAssetDependencyStackPush(XAssetDependencyEntryHeader_t *entry, int size)
{
	++dependenciesStackPointer;

	dependenciesStack[dependenciesStackPointer].dependencylist = entry;
	dependenciesStack[dependenciesStackPointer].size = size;

	dependencies = entry;
	dependenciescounter = size;
}


const char* XAssetTypeToName(XAssetType type)
{
	switch(type)
	{
		case ASSET_TYPE_MENU:
			return "menu";
		case ASSET_TYPE_MENULIST:
			return "menulist";
		case ASSET_TYPE_XANIMPARTS:
			return "xanim";
		default:
			return "default";
	}
}

const char* XAssetTypeToFileExt(XAssetType type)
{
	return XAssetTypeToName(type);
}




static byte *filebuf;

void XAsset_LoadAsset(XAssetType type, const char* assetname)
{
	XAssetFileHeader_t* fileheader;
	int i;

	if ( _setjmp3(error_dropoff, 0) )
	{
		Com_PrintError(CON_FIRST_DEBUG_CHANNEL, "Error loading xasset\n");
		if(filebuf)
		{
			free(filebuf);
		}
		filebuf = NULL;
		return;
	}

	char savepath[256];

	const char* gamedir = FS_GetGameDir();
	if(gamedir[0] == '\0')
	{
		gamedir = "main";
	}

	const char* ext;
	const char* typename;

	ext = XAssetTypeToFileExt(type);
	typename = XAssetTypeToName(type);

	if(Q_stricmp(ext, "default") == 0)
	{
		filebuf = NULL;
		return;
	}
	const char* name = assetname;
	//Load file. Then find XassetType from header and jump to the right loader routine
	Com_sprintf(savepath, sizeof(savepath), "dumps/%s/%s/%s.%s", gamedir, typename, name, ext);

	byte* buf;

	int len = FS_SV_ReadFile( savepath, (void**)&buf );
	if(len <= 0)
	{
		Com_PrintError(CON_FIRST_DEBUG_CHANNEL, "Couldn't load %s from disk\n", savepath);
		filebuf = NULL;
		return;
	}
	if(len <= sizeof(XAssetFileHeader_t) )
	{
		Com_PrintError(CON_FIRST_DEBUG_CHANNEL, "Couldn't load %s from disk. No header.\n", savepath);
		FS_FreeFile(buf);
		filebuf = NULL;
		return;
	}
	//Read the header
	fileheader = (XAssetFileHeader_t*)buf;
	unsigned int stringtabstart = sizeof(XAssetFileHeader_t) + fileheader->numdependencies * sizeof(XAssetDependencyEntryHeader_t);
	//verify it is valid
	if((unsigned int)stringtabstart >= len || (unsigned int)fileheader->headerlen >= len ||
		 (unsigned int)fileheader->datastart + (unsigned int)fileheader->datalen > len)
	{
		Com_PrintError(CON_FIRST_DEBUG_CHANNEL, "Couldn't load %s from disk. Invalid header.\n", savepath);
		FS_FreeFile(buf);
		filebuf = NULL;
		return;
	}

	Com_Printf(CON_FIRST_DEBUG_CHANNEL, "XAsset - Name: %s, Headerlen: %ld, Type: %d, Datastart: %ld, Datalen: %ld, Dependencies: %d\n", name, fileheader->headerlen, fileheader->type, fileheader->datastart, fileheader->datalen, fileheader->numdependencies);
	for(i = 0; i < fileheader->numdependencies; ++i)
	{
		if((unsigned int)fileheader->dependencies[i].name >= (unsigned int)fileheader->datastart)
		{
			Com_PrintError(CON_FIRST_DEBUG_CHANNEL, "Couldn't load %s from disk. Invalid dependency header entry at %d.\n", savepath, i);
			FS_FreeFile(buf);
			filebuf = NULL;
			return;
		}
		fileheader->dependencies[i].name += (stringtabstart + (unsigned int)fileheader);

		Com_Printf(CON_FIRST_DEBUG_CHANNEL, "Depends on %d : %s\n", fileheader->dependencies[i].type, fileheader->dependencies[i].name);

		fileheader->dependencies[i].header = Com_FindXAsset(fileheader->dependencies[i].type, fileheader->dependencies[i].name);
	}

	filebuf = malloc(len);
	memcpy(filebuf, buf + fileheader->datastart, fileheader->datalen);
	Com_Printf(CON_FIRST_DEBUG_CHANNEL, "Loading file: %s\n", savepath);
	MSG_Init(&imagestream, filebuf, len);
	imagestream.cursize = len;

	XAssetDependencyStackPush(fileheader->dependencies, fileheader->numdependencies);

	menuDef_t* menu = MenuLoadMenuDef_t();

	XAssetDependencyStackPop();

	FS_FreeFile(buf);

	if(menu)
	{
		Menu_Open(menu);
	}else{
		Com_Printf(CON_FIRST_DEBUG_CHANNEL, "Error loading file\n");
	}

	filebuf = NULL;
}



void DumpXAsset(XAssetType type, XAssetHeader header)
{

	int len;
	byte headerbuf[1024*32];
	XAssetFileHeader_t* fileheader = (XAssetFileHeader_t*)headerbuf;
	XAssetDependencyEntryHeader_t localdep[1024];
	char savepath[1024];
	const char* typename, *ext, *name;
	void (*dumpproc)(XAssetHeader);

	char* gamedir;

	gamedir = FS_GetGameDir();

	if(gamedir[0] == '\0')
	{
		gamedir = "main";
	}

	ext = XAssetTypeToFileExt(type);
	typename = XAssetTypeToName(type);

	if(Q_stricmp(ext, "default") == 0)
	{
		return;
	}

	switch(type)
	{
		case ASSET_TYPE_MENULIST:
			name = header.menuList->name;
			dumpproc = Dump_MenuList;
			break;
		case ASSET_TYPE_MENU:
			name = header.menu->window.name;
			dumpproc = MenuDumpMenuDef_t;
			break;
		case ASSET_TYPE_XANIMPARTS:
			name = header.parts->name;
			dumpproc = Dump_XAnimParts;
			break;
		default:
			return;
	}

	int dbuflen = 1024*1024*64;

	byte* dump = malloc(dbuflen);
	if(dump == NULL)
	{
		Com_PrintError(CON_FIRST_DEBUG_CHANNEL, "In function DumpXAsset: Error alloc memory\n");
		return;
	}
	dependenciescounter = 0;
	maxdependencies = 1024;
	dependencies = localdep;

	OutputDebugStringA(name);
//	Com_Printf("Name: %s\n", name);

	MSG_Init(&imagestream, dump, dbuflen);

	dumpproc(header);

	len = imagestream.cursize;

	memset(fileheader, 0, sizeof(XAssetFileHeader_t));

	memcpy(fileheader->magic, "IW3XASSET", sizeof(fileheader->magic));
	fileheader->type = type;
	fileheader->numdependencies = dependenciescounter;
	memcpy(fileheader->dependencies, dependencies, fileheader->numdependencies * sizeof(XAssetDependencyEntryHeader_t));
	int stringtabstart = sizeof(XAssetFileHeader_t) + fileheader->numdependencies * sizeof(XAssetDependencyEntryHeader_t);
	int stringtabptr = 0;
	int i;
	for(i = 0; i < fileheader->numdependencies; ++i)
	{
		strcpy((char*)headerbuf + stringtabptr + stringtabstart, fileheader->dependencies[i].name);
		int stringlen = strlen(fileheader->dependencies[i].name);
		fileheader->dependencies[i].name = (char*)stringtabptr;
		stringtabptr += stringlen +1;
	}
	fileheader->headerlen = stringtabstart + stringtabptr;
	fileheader->datastart = fileheader->headerlen + 0;
	fileheader->datalen = len;

	Com_sprintf(savepath, sizeof(savepath), "dumps/%s/%s/%s.%s", gamedir, typename, name, ext);
	fileHandle_t f;
	f = FS_SV_FOpenFileWriteSavePath( savepath );
	if ( !f ) {
    	Com_Printf( CON_FIRST_DEBUG_CHANNEL, "Failed to open %s\n", savepath );
    	return;
	}
	FS_Write( fileheader, fileheader->headerlen, f );
	FS_Write( dump, fileheader->datalen, f );

	FS_FCloseFile( f );
	XAssetFreeDependencyList();
	free(dump);
}



void DumpXAssets( )
{
	int index, sindex, i;
 	XAsset_t *listselector, *slistselect;

	for(i = 0; i < 32768; ++i)
	{
		for(index = db_hashTable[i]; index; index = listselector->nextListIndex)
		{
			listselector = &g_assetEntryPool[index];
/*
			if ( listselector->this.type != type )
				continue;
*/
			DumpXAsset(listselector->this.type, listselector->header);
			for(sindex = listselector->field_C; sindex; sindex = slistselect->field_C)
			{
				slistselect = &g_assetEntryPool[sindex];
				DumpXAsset(listselector->this.type, slistselect->header);
			}
		}
	}

}




/********************************************************

	menuDef procedures

*********************************************************/

/***********************************************************
	Menu Dumper
************************************************************/

void MenuDumpWindowDef_t(windowDef_t* window)
{
	Material_t* default_shader = R_GetDefaultShader();
	//No writing of object because it is not a pointer
	XAssetStoreString(&window->name);
	XAssetStoreString(&window->group);
	if(default_shader == window->background || window->background == NULL || window->background->info.name == NULL)
	{
		window->background = NULL;
	}else{
		window->background = (Material_t*)XAssetAddDependency(ASSET_TYPE_MATERIAL, window->background->info.name, qfalse);
	}
}

void MenuDumpListBoxDef_t(listBoxDef_t** pListbox)
{
	Material_t* default_shader = R_GetDefaultShader();

	listBoxDef_t* cpylistbox = XASSETSTOREPOINTER(pListbox);

	XAssetStoreString(&cpylistbox->doubleClick);

	if(default_shader == cpylistbox->selectIcon || cpylistbox->selectIcon == NULL)
	{
		cpylistbox->selectIcon = NULL;
	}else{
		cpylistbox->selectIcon = (Material_t*)cpylistbox->selectIcon->info.name;
		XAssetStoreString((const char**)&cpylistbox->selectIcon);
	}

}

void MenuDumpEditFieldDef_t(editFieldDef_t** pEditdef)
{
	XASSETSTOREPOINTER(pEditdef);
}

void MenuDumpMultiDef_t(multiDef_t** pMultidef)
{
	int i;
	multiDef_t* cpymultidef = XASSETSTOREPOINTER(pMultidef);;

	for(i = 0; i < 32; ++i)
	{
		XAssetStoreString(&cpymultidef->cvarList[i]);
		XAssetStoreString(&cpymultidef->cvarStr[i]);
	}
}


void MenuDumpItemDefData_t(itemDefData_t* typeData, int type)
{
	switch(type)
	{
		case ITEM_TYPE_LISTBOX:
			MenuDumpListBoxDef_t(&typeData->listBox);
			break;
		case ITEM_TYPE_EDITFIELD:
			MenuDumpEditFieldDef_t(&typeData->editField);
			break;
		case ITEM_TYPE_MULTI:
			MenuDumpMultiDef_t(&typeData->multi);
			break;
		case ITEM_TYPE_DVARENUM:
			XAssetStoreString(&typeData->enumCvarName);
			break;
	}
}


void MenuDumpItemKeyHandler_t(ItemKeyHandler_t** pItemhandler)
{
	if(*pItemhandler == NULL)
	{
		return;
	}
	ItemKeyHandler_t* cpyItemhandler = XASSETSTOREPOINTER(pItemhandler);
	XAssetStoreString(&cpyItemhandler->action);
	MenuDumpItemKeyHandler_t(&cpyItemhandler->next);
}

void MenuDumpStatement_t(statement_t* statement)
{
	int i;
	expressionEntry_t** cpyentries = XAssetStorePointer((void**)&statement->entries, sizeof(expressionEntry_t*) * statement->numEntries);

	for(i = 0; i < statement->numEntries; ++i)
	{
		expressionEntry_t* cpyentry = XAssetStorePointer((void**)&cpyentries[i], sizeof(expressionEntry_t));

		if(cpyentry->type && cpyentry->data.operand.dataType == VAL_STRING)
		{
			XAssetStoreString(&cpyentry->data.operand.internals.string);
		}
	}
}


static void MenuDumpItemDef_t(itemDef_t** pItem, unsigned long parentoffset)
{
	itemDef_t* item = XASSETSTOREPOINTER(pItem);

	XAssetStoreString(&item->text);
	XAssetStoreString(&item->mouseEnterText);
	XAssetStoreString(&item->mouseExitText);
	XAssetStoreString(&item->mouseEnter);
	XAssetStoreString(&item->mouseExit);
	XAssetStoreString(&item->action);
	XAssetStoreString(&item->accept);
	XAssetStoreString(&item->onFocus);
	XAssetStoreString(&item->leaveFocus);
	XAssetStoreString(&item->dvar);
	XAssetStoreString(&item->dvarTest);
	XAssetStoreString(&item->onDvar);

	MenuDumpWindowDef_t(&item->window);

	item->parent = (menuDef_t*)parentoffset;

	MenuDumpItemKeyHandler_t(&item->onKey);

	if(item->focusSound != NULL)
	{
		item->focusSound = (snd_alias_list_t*)XAssetAddDependency(ASSET_TYPE_SOUND, item->focusSound->aliasName, qfalse);
	}
	//Com_TryFindSoundAlias();

	MenuDumpItemDefData_t(&item->typeData, item->type);

	MenuDumpStatement_t(&item->visibleExp);
	MenuDumpStatement_t(&item->textExp);
	MenuDumpStatement_t(&item->materialExp);
	MenuDumpStatement_t(&item->rectXExp);
	MenuDumpStatement_t(&item->rectYExp);
	MenuDumpStatement_t(&item->rectWExp);
	MenuDumpStatement_t(&item->rectHExp);
	MenuDumpStatement_t(&item->foreColorAExp);
}


void MenuDumpMenuDef_t(XAssetHeader header)
{
	int i;

	menuDef_t* menu = header.menu;
	unsigned long parentoffset = 0;
	menuDef_t* cpymenu = XASSETSTOREPOINTER(&menu);

	XAssetStoreString(&cpymenu->font);
	XAssetStoreString(&cpymenu->onOpen);
	XAssetStoreString(&cpymenu->onClose);
	XAssetStoreString(&cpymenu->onESC);
	XAssetStoreString(&cpymenu->allowedBinding);
	XAssetStoreString(&cpymenu->soundLoop);

	MenuDumpWindowDef_t(&cpymenu->window);
	MenuDumpItemKeyHandler_t(&cpymenu->onKey);
	MenuDumpStatement_t(&cpymenu->visibleExp);
	MenuDumpStatement_t(&cpymenu->rectXExp);
	MenuDumpStatement_t(&cpymenu->rectYExp);
	/*MenuDumpItemDef*/
	itemDef_t** cpyitems = XAssetStorePointer((void**)&cpymenu->items, sizeof(itemDef_t*) * cpymenu->itemCount);
	for (i=0; i < cpymenu->itemCount; ++i)
	{
		MenuDumpItemDef_t(&cpyitems[i], parentoffset);
	}
}


/***********************************************************
	Menu Loader
************************************************************/

void MenuLoadItemKeyHandler_t(ItemKeyHandler_t** pItemhandler)
{
	if(*pItemhandler == NULL)
	{
		return;
	}
	if((unsigned int)*pItemhandler >= imagestream.maxsize + sizeof(ItemKeyHandler_t))
	{
			XAssetLoad_Error("MenuLoadItemKeyHandler_t: Memory exceeded\n");
	}
	(*(unsigned int*)pItemhandler) += (unsigned int)imagestream.data;
	XASSETLOADPOINTER(&(*pItemhandler)->action);

	MenuLoadItemKeyHandler_t(&(*pItemhandler)->next);
}


void MenuLoadWindowDef_t(windowDef_t* window)
{
	//No writing of object because it is not a pointer
	XASSETLOADPOINTER(&window->name);
	XASSETLOADPOINTER(&window->group);

	if(window->background != NULL)
	{
		window->background = XAssetGetDependencyAsset((int)window->background).material;
		if(window->background == NULL)
		{
			window->background = R_GetDefaultShader();
		}
	}
}

void MenuLoadStatement_t(statement_t* statement)
{
	int i;

	XASSETLOADPOINTER(&statement->entries);

	for(i = 0; i < statement->numEntries; ++i)
	{
		XASSETLOADPOINTER(&statement->entries[i]);

		if(statement->entries[i]->type)
		{
			if(statement->entries[i]->data.operand.dataType == VAL_STRING)
			{
				XASSETLOADPOINTER(&statement->entries[i]->data.operand.internals.string);
			}
		}
	}
}

void MenuLoadListBoxDef_t(listBoxDef_t** pListbox)
{
	if(*pListbox == NULL)
	{
		return;
	}

	XASSETLOADPOINTER(pListbox);
	XASSETLOADPOINTER(&(*pListbox)->doubleClick);

	if((*pListbox)->selectIcon != NULL)
	{
		(*pListbox)->selectIcon = XAssetGetDependencyAsset((int)(*pListbox)->selectIcon).material;
		if((*pListbox)->selectIcon == NULL)
		{
			(*pListbox)->selectIcon = R_GetDefaultShader();
		}
	}

}
void MenuLoadEditFieldDef_t(editFieldDef_t** pEditdef)
{
	if(*pEditdef == NULL)
	{
		return;
	}

	XASSETLOADPOINTER(pEditdef);
}

void MenuLoadMultiDef_t(multiDef_t** pMultidef)
{
	int i;

	if(*pMultidef == NULL)
	{
		return;
	}

	XASSETLOADPOINTER(pMultidef);

	for(i = 0; i < 32; ++i)
	{
		XASSETLOADPOINTER(&(*pMultidef)->cvarList[i]);
		XASSETLOADPOINTER(&(*pMultidef)->cvarStr[i]);
	}

}

void MenuLoadItemDefData_t(itemDefData_t* typeData, int type)
{

	switch(type)
	{
		case ITEM_TYPE_LISTBOX:
			MenuLoadListBoxDef_t(&typeData->listBox);
			break;
		case ITEM_TYPE_EDITFIELD:
			MenuLoadEditFieldDef_t(&typeData->editField);
			break;
		case ITEM_TYPE_MULTI:
			MenuLoadMultiDef_t(&typeData->multi);
			break;
		case ITEM_TYPE_DVARENUM:
			XASSETLOADPOINTER(&typeData->enumCvarName);
			break;
	}

}

static void MenuLoadItemDef_t(itemDef_t** pItem)
{
	itemDef_t* item;

	if(*pItem == NULL)
	{
		return;
	}

	XASSETLOADPOINTER(pItem);

	item = *pItem;

	XASSETLOADPOINTER(&item->text);
	XASSETLOADPOINTER(&item->mouseEnterText);
	XASSETLOADPOINTER(&item->mouseExitText);
	XASSETLOADPOINTER(&item->mouseEnter);
	XASSETLOADPOINTER(&item->mouseExit);
	XASSETLOADPOINTER(&item->action);
	XASSETLOADPOINTER(&item->accept);
	XASSETLOADPOINTER(&item->onFocus);
	XASSETLOADPOINTER(&item->leaveFocus);
	XASSETLOADPOINTER(&item->dvar);
	XASSETLOADPOINTER(&item->dvarTest);
	XASSETLOADPOINTER(&item->onDvar);

	MenuLoadWindowDef_t(&item->window);

	XASSETLOADZEROPOINTER(&item->parent);

	MenuLoadItemKeyHandler_t(&item->onKey);

	if(item->focusSound != NULL)
	{
		item->focusSound = XAssetGetDependencyAsset((int)item->focusSound).sound;
	}

	//Com_TryFindSoundAlias();

	MenuLoadItemDefData_t(&item->typeData, item->type);


	MenuLoadStatement_t(&item->visibleExp);
	MenuLoadStatement_t(&item->textExp);
	MenuLoadStatement_t(&item->materialExp);
	MenuLoadStatement_t(&item->rectXExp);
	MenuLoadStatement_t(&item->rectYExp);
	MenuLoadStatement_t(&item->rectWExp);
	MenuLoadStatement_t(&item->rectHExp);
	MenuLoadStatement_t(&item->foreColorAExp);

}

menuDef_t* MenuLoadMenuDef_t()
{
	menuDef_t* menu;
	int i;

	if(imagestream.cursize > sizeof(menuDef_t))
	{
		menu = (menuDef_t*)imagestream.data;
	}else{
		return NULL;
	}
	//Get the length of additional memory
	XASSETLOADPOINTER(&menu->font);
	XASSETLOADPOINTER(&menu->onOpen);
	XASSETLOADPOINTER(&menu->onClose);
	XASSETLOADPOINTER(&menu->onESC);
	XASSETLOADPOINTER(&menu->allowedBinding);
	XASSETLOADPOINTER(&menu->soundLoop);
	MenuLoadWindowDef_t(&menu->window);
	MenuLoadItemKeyHandler_t(&menu->onKey);

	MenuLoadStatement_t(&menu->visibleExp);
	MenuLoadStatement_t(&menu->rectXExp);
	MenuLoadStatement_t(&menu->rectYExp);


	/*MenuLoadItemDef*/
	XASSETLOADPOINTER(&menu->items);

	for (i=0; i < menu->itemCount; ++i)
	{
		MenuLoadItemDef_t(&menu->items[i]);
	}
	return menu;
}





/********************************************************

	menuList procedures

*********************************************************/
void Dump_MenuList(XAssetHeader header)
{
	int i;
	MenuList *menulist = header.menuList;
	MenuList *cpymenulist = XASSETSTOREPOINTER(&menulist);
	XAssetStoreString(&cpymenulist->name);

	menuDef_t** cpymenus = XAssetStorePointer((void**)&cpymenulist->menus, sizeof(menuDef_t*) * cpymenulist->menuCount);
	for (i=0; i < cpymenulist->menuCount; ++i)
	{
		cpymenus[i] = (menuDef_t*)XAssetAddDependency(ASSET_TYPE_MENU, cpymenus[i]->window.name, qfalse);
	}
}

MenuList* Load_MenuList( )
{
	MenuList *menulist;
	int i;

	if(imagestream.cursize > sizeof(MenuList))
	{
		menulist = (MenuList*)imagestream.data;
	}else{
		return NULL;
	}

	XASSETLOADPOINTER(&menulist->name);

	XASSETLOADPOINTER(&menulist->menus);


	for(i = 0; i < menulist->menuCount; ++i)
	{
		menulist->menus[i] = XAssetGetDependencyAsset((int)menulist->menus[i]).menu;
	}
	return menulist;
}


/********************************************************

	XAnimParts procedures

*********************************************************/
uint16_t *varScriptString;
XAnimNotifyInfo *varXAnimNotifyInfo;
XAnimDeltaPart* varXAnimDeltaPart;
XAnimPartTrans *varXAnimPartTrans;
XAnimDeltaPartQuat *varXAnimDeltaPartQuat;
XAnimPartTransData *varXAnimPartTransData;
XAnimPartTransFrames *varXAnimPartTransFrames;
XAnimDynamicIndices *varXAnimDynamicIndicesTrans;

#define varXAssetList (*(struct XAssetList **)0xe34688)

/*
void Load_ScriptStringCustom(uint16_t *var)
{
  *var = (uint16_t)varXAssetList->stringList.strings[*var];
}

void Load_ScriptStringArray(int count)
{
	//LoadPointer maybe?
	int i;
	uint16_t *s;
	for(i = 0, s = varScriptString; i < count; ++i, ++s)
	{
		Load_ScriptStringCustom(s);
	}
}
*/

void Dump_ScriptStringCustom(uint16_t *var)
{
	/*
	//Com_Printf("Want %d\n", *var);
	int i;
	for(i = 0; i < varXAssetList->stringList.count; ++i)
	{
		if(varXAssetList->stringList.strings[i] == *var)
		{
	//		Com_Printf("Found: %d at %d\n", *var, i);
			*var = i;
			return;
		}
	}
//	Com_PrintError("Dump_ScriptStringCustom: Var not found %d\n", *var);
*/
	*var = 0;

}

void Dump_ScriptStringArray(int count)
{
	int i;
	for(i = 0; i < count; ++i)
	{
		Dump_ScriptStringCustom(&varScriptString[i]);
	}
}

void Dump_XAnimNotifyInfoArray(int count)
{
	int i;
	for(i = 0; i < count; ++i)
	{
		Dump_ScriptStringCustom(&varXAnimNotifyInfo[i].name);
	}
}


void Dump_XAnimDeltaPartTrans()
{
	varXAnimPartTransData = &varXAnimPartTrans->u;
	if(varXAnimPartTrans->size)
	{
		varXAnimPartTransFrames = &varXAnimPartTrans->u.frames;
		varXAnimDynamicIndicesTrans = &varXAnimPartTransFrames->indices;
	}

}
void Dump_XAnimDeltaPartQuat()
{

}

void Dump_XAnimDeltaPart()
{
	if(varXAnimDeltaPart->trans)
	{
		varXAnimPartTrans = XASSETSTOREPOINTER(&varXAnimDeltaPart->trans);
		Dump_XAnimDeltaPartTrans();
	}
	if(varXAnimDeltaPart->quat)
	{
		varXAnimDeltaPartQuat = XASSETSTOREPOINTER(&varXAnimDeltaPart->quat);
		Dump_XAnimDeltaPartQuat();
	}
}


void Dump_XAnimParts(XAssetHeader header)
{
	Com_Printf(CON_FIRST_DEBUG_CHANNEL, "Dumping xanim %s\n", header.parts->name);

	XAnimParts* parts = header.parts;
	XAnimParts* varXAnimParts = XASSETSTOREPOINTER(&parts);
	XAssetStoreString(&varXAnimParts->name);

	if ( varXAnimParts->names )
	{
		int count = varXAnimParts->boneCount[11];
		varScriptString = XAssetStorePointer((void**)&varXAnimParts->names, sizeof(uint16_t*) * count);
		Dump_ScriptStringArray(count); //ToDo fix scriptstrings
	}
	if ( varXAnimParts->notify )
	{
		int count = varXAnimParts->notifyCount;
		varXAnimNotifyInfo = XAssetStorePointer((void**)&varXAnimParts->notify, sizeof(XAnimNotifyInfo*) * count);
		Dump_ScriptStringArray(count); //ToDo fix scriptstrings
	}
	if(varXAnimParts->deltaPart)
	{
		XASSETSTOREPOINTER(&varXAnimParts->deltaPart);
		varXAnimDeltaPart = varXAnimParts->deltaPart;
		Dump_XAnimDeltaPart();
	}
}

/*
void Dump_XAnim(XAnim_s* anim)
{
	int i, k;

	int numadditionalentries = anim->size -1;
	if(numadditionalentries  < 0)
	{
		numadditionalentries = 0;
	}

	XAnim_s* cpyanim = XAssetStorePointer((void**)&anim, sizeof(*anim) + numadditionalentries * sizeof(anim->entries[0]));

	XAssetStoreString(&cpyanim->debugName);
	for(i = 0; cpyanim->debugAnimNames[i]; ++i);

	char** cpydebugAnimNames = XAssetStorePointer((void**)&cpyanim->debugAnimNames, sizeof(char**) * i +1);

	for (i = 0; i < cpyanim->size; ++i)
	{
		if(cpyanim->entries[i].numAnims > 0)
		{
			XAnimDumpXAnimParts(&cpyanim->entries[i].u.parts);
		}
	}
}
*/


void XAsset_test_f()
{
	DumpXAssets();
}

void XAsset_stringlist_f()
{

	int i;

	for(i = 0; i < varXAssetList->stringList.count; ++i)
	{
		if(varXAssetList->stringList.strings[i])
		{
			Com_Printf(CON_FIRST_DEBUG_CHANNEL, "ScriptString Index: %hu\n", varXAssetList->stringList.strings[i]);
		}
	}

}


void XAssetLoader_Init()
{
	//Cmd_AddCommand("xassettest", XAsset_test_f);
	//Cmd_AddCommand("scriptstringlist", XAsset_stringlist_f);

}
