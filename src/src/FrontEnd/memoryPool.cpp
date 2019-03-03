#include <memory.h>  
#include "memoryPool.h"  
/************************************************************************/  
/* 内存池起始地址对齐到ADDR_ALIGN字节 
/************************************************************************/  
size_t check_align_addr(void*& pBuf)  
{  
    size_t align = 0;  
    size_t addr = (long)pBuf;  
    align = (ADDR_ALIGN - addr % ADDR_ALIGN) % ADDR_ALIGN;  
    pBuf = (char*)pBuf + align;  
    return align;  
}  
/************************************************************************/  
/* 内存block大小对齐到MINUNITSIZE字节 
/************************************************************************/  
size_t check_align_block(size_t size)  
{  
    size_t align = size % MINUNITSIZE;  
      
    return size - align;   
}  
/************************************************************************/  
/* 分配内存大小对齐到SIZE_ALIGN字节 
/************************************************************************/  
size_t check_align_size(size_t size)  
{  
    size = (size + SIZE_ALIGN - 1) / SIZE_ALIGN * SIZE_ALIGN;  
    return size;  
}  
/************************************************************************/  
/* 以下是链表相关操作 
/************************************************************************/  
memory_chunk* create_list(memory_chunk* pool, size_t count)  
{  
    if (!pool)  
    {  
        return NULL;  
    }  
    memory_chunk* head = NULL;  
    for (size_t i = 0; i < count; i++)  
    {  
        pool->pre = NULL;  
        pool->next = head;  
        if (head != NULL)  
        {  
            head->pre = pool;              
        }  
        head = pool;  
        pool++;  
    }  
    return head;  
}  
memory_chunk* front_pop(memory_chunk*& pool)  
{  
    if (!pool)  
    {  
        return NULL;  
    }  
    memory_chunk* tmp = pool;  
    pool = tmp->next;  
    pool->pre = NULL;  
    return  tmp;  
}  
void push_back(memory_chunk*& head, memory_chunk* element)  
{  
    if (head == NULL)  
    {  
        head = element;  
        head->pre = element;  
        head->next = element;  
        return;  
    }  
    head->pre->next = element;  
    element->pre = head->pre;  
    head->pre = element;  
    element->next = head;  
}  
void push_front(memory_chunk*& head, memory_chunk* element)  
{  
    element->pre = NULL;  
    element->next = head;  
    if (head != NULL)  
    {  
        head->pre = element;           
    }  
    head = element;  
}  
void delete_chunk(memory_chunk*& head, memory_chunk* element)  
{  
    // 在双循环链表中删除元素  
    if (element == NULL)  
    {  
        return;  
    }  
    // element为链表头  
    else if (element == head)  
    {  
        // 链表只有一个元素  
        if (head->pre == head)  
        {  
            head = NULL;  
        }  
        else  
        {  
            head = element->next;  
            head->pre = element->pre;  
            head->pre->next = head;  
        }  
    }  
    // element为链表尾  
    else if (element->next == head)  
    {  
        head->pre = element->pre;  
        element->pre->next = head;  
    }  
    else  
    {  
        element->pre->next = element->next;  
        element->next->pre = element->pre;  
    }  
    element->pre = NULL;  
    element->next = NULL;  
}  
/************************************************************************/  
/* 内存映射表中的索引转化为内存起始地址                                                                     
/************************************************************************/  
void* index2addr(PMEMORYPOOL mem_pool, size_t index)  
{  
    char* p = (char*)(mem_pool->memory);  
    void* ret = (void*)(p + index *MINUNITSIZE);  
      
    return ret;  
}  
/************************************************************************/  
/* 内存起始地址转化为内存映射表中的索引                                                                     
/************************************************************************/  
size_t addr2index(PMEMORYPOOL mem_pool, void* addr)  
{  
    char* start = (char*)(mem_pool->memory);  
    char* p = (char*)addr;  
    size_t index = (p - start) / MINUNITSIZE;  
    return index;  
}  
/************************************************************************/  
/* 生成内存池 
* pBuf: 给定的内存buffer起始地址 
* sBufSize: 给定的内存buffer大小 
* 返回生成的内存池指针 
/************************************************************************/  
PMEMORYPOOL CreateMemoryPool(void* pBuf, size_t sBufSize)  
{  
    memset(pBuf, 0, sBufSize);  
    PMEMORYPOOL mem_pool = (PMEMORYPOOL)pBuf;  
    // 计算需要多少memory map单元格  
    size_t mem_pool_struct_size = sizeof(MEMORYPOOL);  
    mem_pool->mem_map_pool_count = (sBufSize - mem_pool_struct_size + MINUNITSIZE - 1) / MINUNITSIZE;  
    mem_pool->mem_map_unit_count = (sBufSize - mem_pool_struct_size + MINUNITSIZE - 1) / MINUNITSIZE;  
    mem_pool->pmem_map = (memory_block*)((char*)pBuf + mem_pool_struct_size);  
    mem_pool->pfree_mem_chunk_pool = (memory_chunk*)((char*)pBuf + mem_pool_struct_size + sizeof(memory_block) * mem_pool->mem_map_unit_count);  
      
    mem_pool->memory = (char*)pBuf + mem_pool_struct_size+ sizeof(memory_block) * mem_pool->mem_map_unit_count + sizeof(memory_chunk) * mem_pool->mem_map_pool_count;  
    mem_pool->size = sBufSize - mem_pool_struct_size - sizeof(memory_block) * mem_pool->mem_map_unit_count - sizeof(memory_chunk) * mem_pool->mem_map_pool_count;  
    size_t align = check_align_addr(mem_pool->memory);  
    mem_pool->size -= align;  
    mem_pool->size = check_align_block(mem_pool->size);  
    mem_pool->mem_block_count = mem_pool->size / MINUNITSIZE;  
    // 链表化  
    mem_pool->pfree_mem_chunk_pool = create_list(mem_pool->pfree_mem_chunk_pool, mem_pool->mem_map_pool_count);  
    // 初始化 pfree_mem_chunk，双向循环链表  
    memory_chunk* tmp = front_pop(mem_pool->pfree_mem_chunk_pool);  
    tmp->pre = tmp;  
    tmp->next = tmp;  
    tmp->pfree_mem_addr = NULL;  
    mem_pool->mem_map_pool_count--;  
      
    // 初始化 pmem_map  
    mem_pool->pmem_map[0].count = mem_pool->mem_block_count;  
    mem_pool->pmem_map[0].pmem_chunk = tmp;  
    mem_pool->pmem_map[mem_pool->mem_block_count-1].start = 0;  
      
    tmp->pfree_mem_addr = mem_pool->pmem_map;  
    push_back(mem_pool->pfree_mem_chunk, tmp);  
    mem_pool->free_mem_chunk_count = 1;  
    mem_pool->mem_used_size = 0;  
    return mem_pool;  
}  
/************************************************************************/  
/* 暂时没用 
/************************************************************************/  
void ReleaseMemoryPool(PMEMORYPOOL* ppMem)   
{  
}  
/************************************************************************/  
/* 从内存池中分配指定大小的内存  
* pMem: 内存池 指针 
* sMemorySize: 要分配的内存大小 
* 成功时返回分配的内存起始地址，失败返回NULL 
/************************************************************************/  
void* GetMemory(size_t sMemorySize, PMEMORYPOOL pMem)  
{  
    sMemorySize = check_align_size(sMemorySize);  
    size_t index = 0;  
    memory_chunk* tmp = pMem->pfree_mem_chunk;  
    for (index = 0; index < pMem->free_mem_chunk_count; index++)  
    {  
        if (tmp->pfree_mem_addr->count * MINUNITSIZE >= sMemorySize)  
        {             
            break;  
        }  
          
        tmp = tmp->next;  
    }  
      
    if (index == pMem->free_mem_chunk_count)  
    {  
        return NULL;  
    }  
    pMem->mem_used_size += sMemorySize;  
    if (tmp->pfree_mem_addr->count * MINUNITSIZE == sMemorySize)  
    {  
        // 当要分配的内存大小与当前chunk中的内存大小相同时，从pfree_mem_chunk链表中删除此chunk  
        size_t current_index = (tmp->pfree_mem_addr - pMem->pmem_map);  
        delete_chunk(pMem->pfree_mem_chunk, tmp);  
        tmp->pfree_mem_addr->pmem_chunk = NULL;  
          
        push_front(pMem->pfree_mem_chunk_pool, tmp);  
        pMem->free_mem_chunk_count--;  
        pMem->mem_map_pool_count++;  
          
        return index2addr(pMem, current_index);  
    }  
    else  
    {  
        // 当要分配的内存小于当前chunk中的内存时，更改pfree_mem_chunk中相应chunk的pfree_mem_addr  
          
        // 复制当前mem_map_unit  
        memory_block copy;  
        copy.count = tmp->pfree_mem_addr->count;  
        copy.pmem_chunk = tmp;  
        // 记录该block的起始和结束索引  
        memory_block* current_block = tmp->pfree_mem_addr;  
        current_block->count = sMemorySize / MINUNITSIZE;  
        size_t current_index = (current_block - pMem->pmem_map);  
        pMem->pmem_map[current_index+current_block->count-1].start = current_index;  
        current_block->pmem_chunk = NULL; // NULL表示当前内存块已被分配  
        // 当前block被一分为二，更新第二个block中的内容  
        pMem->pmem_map[current_index+current_block->count].count = copy.count - current_block->count;  
        pMem->pmem_map[current_index+current_block->count].pmem_chunk = copy.pmem_chunk;  
        // 更新原来的pfree_mem_addr  
        tmp->pfree_mem_addr = &(pMem->pmem_map[current_index+current_block->count]);  
      
        size_t end_index = current_index + copy.count - 1;  
        pMem->pmem_map[end_index].start = current_index + current_block->count;  
        return index2addr(pMem, current_index);  
    }     
}  
/************************************************************************/  
/* 从内存池中释放申请到的内存 
* pMem：内存池指针 
* ptrMemoryBlock：申请到的内存起始地址 
/************************************************************************/  
void FreeMemory(void *ptrMemoryBlock, PMEMORYPOOL pMem)   
{  
    size_t current_index = addr2index(pMem, ptrMemoryBlock);  
    size_t size = pMem->pmem_map[current_index].count * MINUNITSIZE;  
    // 判断与当前释放的内存块相邻的内存块是否可以与当前释放的内存块合并  
    memory_block* pre_block = NULL;  
    memory_block* next_block = NULL;  
    memory_block* current_block = &(pMem->pmem_map[current_index]);  
    // 第一个  
    if (current_index == 0)  
    {  
        if (current_block->count < pMem->mem_block_count)  
        {  
            next_block = &(pMem->pmem_map[current_index+current_block->count]);  
            // 如果后一个内存块是空闲的，合并  
            if (next_block->pmem_chunk != NULL)  
            {  
                next_block->pmem_chunk->pfree_mem_addr = current_block;  
                pMem->pmem_map[current_index+current_block->count+next_block->count-1].start = current_index;  
                current_block->count += next_block->count;  
                current_block->pmem_chunk = next_block->pmem_chunk;  
                next_block->pmem_chunk = NULL;  
            }  
            // 如果后一块内存不是空闲的，在pfree_mem_chunk中增加一个chunk  
            else  
            {  
                memory_chunk* new_chunk = front_pop(pMem->pfree_mem_chunk_pool);  
                new_chunk->pfree_mem_addr = current_block;  
                current_block->pmem_chunk = new_chunk;  
                push_back(pMem->pfree_mem_chunk, new_chunk);  
                pMem->mem_map_pool_count--;  
                pMem->free_mem_chunk_count++;  
            }  
        }  
        else  
        {  
            memory_chunk* new_chunk = front_pop(pMem->pfree_mem_chunk_pool);  
            new_chunk->pfree_mem_addr = current_block;  
            current_block->pmem_chunk = new_chunk;  
            push_back(pMem->pfree_mem_chunk, new_chunk);  
            pMem->mem_map_pool_count--;  
            pMem->free_mem_chunk_count++;  
        }         
    }  
      
    // 最后一个  
    else if (current_index == pMem->mem_block_count-1)  
    {  
        if (current_block->count < pMem->mem_block_count)  
        {  
            pre_block = &(pMem->pmem_map[current_index-1]);  
            size_t index = pre_block->count;  
            pre_block = &(pMem->pmem_map[index]);  
              
            // 如果前一个内存块是空闲的，合并  
            if (pre_block->pmem_chunk != NULL)  
            {  
                pMem->pmem_map[current_index+current_block->count-1].start = current_index - pre_block->count;  
                pre_block->count += current_block->count;  
                current_block->pmem_chunk = NULL;  
            }  
            // 如果前一块内存不是空闲的，在pfree_mem_chunk中增加一个chunk  
            else  
            {  
                memory_chunk* new_chunk = front_pop(pMem->pfree_mem_chunk_pool);  
                new_chunk->pfree_mem_addr = current_block;  
                current_block->pmem_chunk = new_chunk;  
                push_back(pMem->pfree_mem_chunk, new_chunk);  
                pMem->mem_map_pool_count--;  
                pMem->free_mem_chunk_count++;  
            }  
        }  
        else  
        {  
            memory_chunk* new_chunk = front_pop(pMem->pfree_mem_chunk_pool);  
            new_chunk->pfree_mem_addr = current_block;  
            current_block->pmem_chunk = new_chunk;  
            push_back(pMem->pfree_mem_chunk, new_chunk);  
            pMem->mem_map_pool_count--;  
            pMem->free_mem_chunk_count++;  
        }  
    }  
    else  
    {         
        next_block = &(pMem->pmem_map[current_index+current_block->count]);  
        pre_block = &(pMem->pmem_map[current_index-1]);  
        size_t index = pre_block->start;  
        pre_block = &(pMem->pmem_map[index]);  
        bool is_back_merge = false;  
        if (next_block->pmem_chunk == NULL && pre_block->pmem_chunk == NULL)  
        {  
            memory_chunk* new_chunk = front_pop(pMem->pfree_mem_chunk_pool);  
            new_chunk->pfree_mem_addr = current_block;  
            current_block->pmem_chunk = new_chunk;  
            push_back(pMem->pfree_mem_chunk, new_chunk);  
            pMem->mem_map_pool_count--;  
            pMem->free_mem_chunk_count++;  
        }  
        // 后一个内存块  
        if (next_block->pmem_chunk != NULL)  
        {  
            next_block->pmem_chunk->pfree_mem_addr = current_block;  
            pMem->pmem_map[current_index+current_block->count+next_block->count-1].start = current_index;  
            current_block->count += next_block->count;  
            current_block->pmem_chunk = next_block->pmem_chunk;  
            next_block->pmem_chunk = NULL;  
            is_back_merge = true;  
        }  
        // 前一个内存块  
        if (pre_block->pmem_chunk != NULL)  
        {  
            pMem->pmem_map[current_index+current_block->count-1].start = current_index - pre_block->count;  
            pre_block->count += current_block->count;  
            if (is_back_merge)  
            {  
                delete_chunk(pMem->pfree_mem_chunk, current_block->pmem_chunk);  
                push_front(pMem->pfree_mem_chunk_pool, current_block->pmem_chunk);  
                pMem->free_mem_chunk_count--;  
                pMem->mem_map_pool_count++;  
            }  
            current_block->pmem_chunk = NULL;              
        }         
    }  
    pMem->mem_used_size -= size;  
}  