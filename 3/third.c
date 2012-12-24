#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main(EFI_HANDLE hndl, EFI_SYSTEM_TABLE *systab)
{
	InitializeLib(hndl, systab);
	EFI_MEMORY_DESCRIPTOR *mmap;
	EFI_STATUS status;
	int one_page_size = 4096;
	UINTN mem_hldr, desc_size, map_key, mmap_size;
	UINT32 desc_vers;
	EFI_MEMORY_TYPE mem_type = EfiLoaderData;
	
	status = uefi_call_wrapper(systab->BootService->GetMemoryMap, 5, &mmap_size, mmap, &map_key, &desc_size, &desc_vers);
	if(status == EFI_BUFFER_TOO_SMALL)
	{
		status = uefi_call_wrapper(systab->BootService->AllocatePool, 3, mem_type, mmap_size, &mmap);  
		if(status != EFI_SUCCESS)
		{
			Print(L"Can't allocate memory\n");
			return EFI_SUCCESS;
		}
		status = uefi_call_wrapper(systab->BootService->GetMemoryMap, 5, &mmap_size, mmap, &map_key, &desc_size, &desc_vers);  
		if(status != EFI_SUCCESS)
		{
			Print(L"Can't get memory map\n");
			return EFI_SUCCESS;
		}
		for (int i=0; i<mmap_size/(sizeof(EFI_MEMORY_DESCRIPTOR)); i++)
		{
			if((mmap[i].Type == EfiConventionalMemory)||(mmap[i].Type == EfiBootServicesData)||(mmap[i].Type == EfiBootServicesCode))
			{
				mem_hldr += mmap[i].NumberOfPages;
			}
			mem_hldr *= one_page_size;
		}
		Print(L"Free memory is equal to %d bytes\n",mem_hldr);
		status = uefi_call_wrapper(systab->BootService->FreePool, 1, &mmap);		
	}
	else
	{
		Print (L"Can't get memory map\n");
	}
	return EFI_SUCCESS;
}