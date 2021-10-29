#pragma once

void itoa(int number, char* buffer, int base)
{
    switch (base)
    {
        case 10:
            sprintf(buffer, "%d", number);
            break;
        case 16:
            sprintf(buffer, "%x", number);
            break;
        default:
            printf("Unsupported base in itoa");
            exit(1);
            break;
    }
}

void lltoa(long long number, char* buffer, int base)
{
    switch (base)
    {
        case 10:
            sprintf(buffer, "%lld", number);
            break;
        case 16:
            sprintf(buffer, "%llx", number);
            break;
        default:
            printf("Unsupported base in lltoa");
            exit(1);
            break;
    }
}
