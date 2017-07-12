#include "calendar.h"
void calendar_handler(int32_t month, int32_t year)
{
	if(year < MIN_YEAR || month < 1 || month > 12)
		return;
	else
	{
		int32_t monthly_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		char *monthly_titles[12] = {"January\n", "February\n", "March\n", "April\n", "May\n", "June\n", "July\n", "August\n", "September\n", "October\n", "November\n", "December\n"};
		//char *weekly_titles[7] = {"Su", "Mo", "Tu", "We", "Tu", "Fr", "Sa"};
		//int32_t f = 0;
		term_write(1, (int8_t *)(monthly_titles[month-1]), strlen(monthly_titles[month-1]));
		term_write(1, "Su Mo Tu We Th Fr Sa\n", 21);
		//printf("Su Mo Tu We Tu Fr Sa");
		//printf("\n");
		int32_t dotw = 1;
		int32_t i, j, k;
		char buf[SIZE];
		for(i = MIN_YEAR; i <= year; i++){
			for(j = 0; j < MONTHS; j++)
			{
				int32_t dotm = 1;
				int32_t lim = monthly_days[j];
				int32_t index = 0;
				if(((i - MIN_YEAR) % LEAP == 0) && j == 1)
					lim += 1;
				for(k = 0; k < lim; k++)
				{
					if(i == year && j+1 == month)
					{
						//add to the string
						if(dotm < DIV_BASE)
						{
							if(dotm == 1 && dotw != 1)
							{
								int32_t g;
								for(g = 0; g < dotw-1; g++)
								{
									buf[0+IDX_OFF*g] = ' ';
									buf[1+IDX_OFF*g] = ' ';
									buf[2+IDX_OFF*g] = ' ';
									index += IDX_OFF;
								}
							}
							buf[index] = '0';
							buf[index+1] = '0'+dotm;
							buf[index+2] = ' ';
							index += IDX_OFF;
						}
						else
						{
							buf[index] = '0'+dotm/DIV_BASE;
							buf[index+1] = '0'+dotm%DIV_BASE;
							buf[index+2] = ' ';
							index += IDX_OFF;
						}
					}
					dotw += 1;
					dotm += 1;
					if(dotw > 7)
					{
						dotw = 1;
						if(i == year && j+1 == month)
						{
							//append newline, print and clear string
							buf[SIZE-2] = '\n';
							//buf[21] = '\0';
							term_write(1, buf, SIZE-1);
							index = 0;
							int32_t e;
							for(e = 0; e < SIZE; e++)
								buf[e] = ' ';
						}
					}
				}
				if(index != 0)
				{
					buf[SIZE-2] = '\n';
					//buf[21] = '\0';
					term_write(1, buf, SIZE-1);
				}
			}
		}
	}
}
