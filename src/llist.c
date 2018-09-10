#include "cdebug.h"

#include "combine.h"
#include "gdfonts.h"
#include "count.h"

/* private protos */
static Sllist *initList _Declare ((void));

/*
**  initList()
**  mallocs and initializes the Sllist struct
**
**  RCS
**      $Revision: 1.1.1.1 $
**      $Date: 2001/03/19 01:59:51 $
**  Return Values:
**      pointer to struct Sllist        if succeeds
**      NULL                            if fails
**
**  Parameters:
**      none
**
**  Side Effects:
**      memory is allocated
**
**  Limitations and Comments:
**      caller should check the return value before using it
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-18-1997    first cut
*/

static Sllist *initList()
{
    Sllist
        *ll;

    ll=(Sllist *) malloc(sizeof(Sllist));
    if (ll == (Sllist *) NULL)
    {
        return((Sllist *) NULL);
    }
    ll->item=(char *) NULL;

    return(ll);
}
/*
**  additemToList()
**  add item to a list. 
**
**  RCS
**      $Revision: 1.1.1.1 $
**      $Date: 2001/03/19 01:59:51 $
**  Return Values:
**      0       if succeeds
**      -1      if fails
**
**  Parameters:
**      head        pointer to pointer to Sllist
**      item        the string to add
**
**  Side Effects:
**      head is modified
**
**  Limitations and Comments:
**      must be called from a loop. head must be initialized to NULL when
**      called first time.
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-18-1997    frist cut
*/

int additemToList(head,item)
Sllist
    **head;
char
    *item;
{
    Sllist
        *ll;
    static Sllist
        *tmp;

    if (item == (char *) NULL)
        return (-1);


    /* malloc for the struct */
    ll=initList();
    if (ll == (Sllist *) NULL)
    {
        return (-1);
    }

    /* 
    ** we'r not checking status of mystrdup(), because if it fails, we'll exit
    ** from it
    */
    ll->item=mystrdup(item);

    if ((*head) == (Sllist *) NULL) /* first time */
    {
        (*head)=ll;
        ll->next=(Sllist *) NULL;
        tmp=ll;
    }
    else
    {
        tmp->next=ll;
        ll->next=(Sllist *) NULL;
        tmp=ll;
    }

    return (0);
}
/*
** releaseList()
** frees the item and finally the list itself
**
**  RCS
**      $Revision: 1.1.1.1 $
**      $Date: 2001/03/19 01:59:51 $
**  Return Values:
**      none
**
**  Parameters:
**      head        pointer to pointer to Sllist
**
**  Side Effects:
**      the list is freed and initialized to NULL
**
**  Limitations and Comments:
**      never use the list after calling this routine
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-18-1997    first cut
*/

void releaseList(head)
Sllist
    **head;
{
    Sllist
        *ptr;

    while (*head)
    {
        ptr= (*head);
        (*head)=(*head)->next;

        if ((*head)->item)
        {
            (void) free ((char *) ((*head)->item));
            (*head)->item=(char *) NULL;
        }
        (void) free((char *) ptr);
    }
}
/*
**  printList()
**  just prints the intem in the Sllist. this routine is for debug
**  purpose only
**
**  RCS
**      $Revision: 1.1.1.1 $
**      $Date: 2001/03/19 01:59:51 $
**  Return Values:
**      none
**
**  Parameters:
**      head        pointer to pointer to Sllist
**
**  Side Effects:
**      none
**
**  Limitations and Comments:
**      none
**
**  Development History:
**      who                  when           why
**      ma_muquit@fccc.edu   Oct-18-1997    first cut
*/

void printList(head)
Sllist
    *head;
{
    Sllist
        *ptr;

    for (ptr=head;ptr;ptr=ptr->next)
    {
        if (ptr->item)
        {
            Debug2("item=%s",ptr->item,0);
        }
        else
        {
            Debug2("no item found",0,0);
        }
    }
}
