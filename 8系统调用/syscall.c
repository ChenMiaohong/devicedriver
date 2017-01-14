void pk()
{
    __asm__ (
    "ldr r7,=363 \n"
    "swi \n"
    :
    :
    :"memory");
}

void main()
{
    pk();	
}