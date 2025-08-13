#include <semaphore.h>

static sem_t semaphoreList[SEM_NUM] = {0};

sem_t *sys_sem_open(const char *name, unsigned int value)
{
    int i;
    char c;
    char newName[NAME_SIZE] = {0};
    if (!get_fs_byte(name))
        return NULL;
    for (i = 0; i < NAME_SIZE - 1; i++)
    {
        c = get_fs_byte(name + i);
        if (c == '\0')
            break;
        else
            newName[i] = c;
    }
    newName[i] = 0;
    for (i = 0; i < SEM_NUM; i++)
    {
        if (!strcmp(newName, semaphoreList[i].name))
        {
            return &semaphoreList[i];
        }
    }
    for (i = 0; i < SEM_NUM; i++)
    {
        if (!semaphoreList[i].name[0])
        {
            strcpy(semaphoreList[i].name, newName);
            semaphoreList[i].value = value;
            semaphoreList[i].queue = NULL;
            return &semaphoreList[i];
        }
    }
    return NULL;
}

int sys_sem_wait(sem_t *sem)
{
    cli();
    if (sem < semaphoreList || sem >= semaphoreList + SEM_NUM)
    {
        printk("error sem");
        sti();
        return -1;
    }
    while (sem->value <= 0)
        sleep_on(&(sem->queue));
    sem->value--;
    sti();
    return 0;
}

int sys_sem_post(sem_t *sem)
{
    cli();
    if (sem < semaphoreList || sem >= semaphoreList + SEM_NUM)
    {
        printk("error sem");
        sti();
        return -1;
    }
    ++sem->value;
    wake_up(&(sem->queue));
    sti();
    return 0;
}

int sys_sem_unlink(const char *name)
{
    int i;
    char c;
    char newName[NAME_SIZE] = {0};
    if (!get_fs_byte(name))
        return -1;
    for (i = 0; i < NAME_SIZE - 1; i++)
    {
        c = get_fs_byte(name + i);
        if (c == '\0')
            break;
        else
            newName[i] = c;
    }
    newName[i] = 0;
    for (i = 0; i < SEM_NUM; i++)
    {
        if (!strcmp(newName, semaphoreList[i].name))
        {
            semaphoreList[i].name[0] = '\0';
            return 0;
        }
    }
    return -1;
}