/********************************************

    Car Database C Program
    [Evaluated]

    By: Dima Ivanov
    11/12/2017 - 15/12/2017
    ICS3U (Ms. Payne)

    A program designed to:

    - Store information about cars
    - load information about cars
    - sort information about cars
    - display information about cars
    - add information about cars
    - remove information about cars
    - edit information about cars


    TO DO:
    - create cars
    - implement car management into displayMenu
    - filter by ranges???
    - display calculated price


********************************************/

#include<stdio.h>

/* Structures */

typedef struct //car
{
    //for sorting types, see "sortData" function
    //odd number = alphabetical / lesser-greater value
    //even number = rev. alphabetical / greater - lesser value

    char make[31]; // sorting type = 1-2
    char model[31]; // sorting type = 3-4
    char ownerFName[31]; //No sorting type, that's creepy (but it would work exactly the same as make and model)
    char ownerLName[31]; //No sorting type, that's creepy (but it would work exactly the same as make and model)
    int year; // sorting type = 5-6
    int initialPriceDollars; // sorting type = 7-8
    int mileage; // sorting type = 9-10
}
car;

/* Global Variables */
car carList[100]; //The database stores 100 cars, no new cars can be added if the database is full
int numCars; //How many cars are in the database, used in several functions

/* Function Prototypes */
//See individual function declarations for explanations
int displayTitle(void);
int displayClosingScreen(void);
void displayCarPages(void);
void editCar(int);
void removeCar(int);
void createCar(void);
int loadData(void);
int saveData(void);
void sortData(int);
void displaySortingMenu(void);
void switchCars(car*,car*);

/* MAIN */
int main(void)
{
    //calls loadData, if it is unsuccessful, it displays a message and exits
    if(loadData()==0)
    {
        printf("\n\nFailed to load data, program unavailable\nPlease check that \"cars.txt\" exists");
        getch();
        return 0;
    }

    //testing
    removeCar(0);

    //loop 1, runs once, loops while the closing screen does not return 1 for user exiting
    do
    {
        //loop 2, displays title while the user does not exit the title
        while(displayTitle()==1)
        {
            displayCarPages(); //called if user passes title
        }
    }
    while(displayClosingScreen()==0);

    //all loops have broken, program exits

    return 0;
}

/* Function Declarations */

/*
Function: displayTitle
Does: Shows the title and waits for an input, displays closing message if the user exited
Uses: nothing
Receives: nothing
Returns: integer 1 or 0 based on whether or not the user exited the program
*/
int displayTitle(void)
{
    system("cls");
    //title display
    printf("=====\nTitle\n=====\n\nPress to continue\nEsc - Exit");
    //either the input is esc or it's not (exit or enter)
    if(getch()==27)
    {
        return 0; //user exited
    }
    else
    {
        return 1; //user entered
    }
}

/*
Function: displayClosingScreen
Does: Asks the user whether they would like to save, not save, or return to the program
Uses:

- options (char[][]): a string array of all the options available to user, simplifies printing to a for loop

- input (char): stores user's input for repeated use

- selection (int): stores the location of the user's cursor

- i (int): miscellaneous counter, mainly used for printing options

Receives: nothing
Returns: integer 0 or 1 based on whether or not user exited (save function is called in function itself)
*/
int displayClosingScreen(void)
{
    //variables
    char options[3][20]={"Save","Return","Do Not Save"};
    char input;
    int selection=0;
    int i;

    //loops while input is neither esc or backspace, nor enter (enter breaks because the function would return anyways after the user presses enter and something happened as a result)
    do
    {
        system("cls");
        printf("==================\nSave Before Exiting?\n==================\n\n");
        //options prnting loop
        for(i=0;i<3;i++)
        {
            if(i==selection)
            {
                printf(" > "); //option is selected
            }
            else
            {
                printf("   "); //option is not selected
            }
            printf("%s\n",options[i]); //printing option
        }
        input=getch(); //getting input
        //doing stuff with input:
        switch(input)
        {
            case 72:
                //move cursor up if it can
                if(selection>0)
                {
                    selection--;
                }
                break;
            case 80:
                //move cursor down if it can
                if(selection<2)
                {
                    selection++;
                }
                break;
            case 13:
                //user pressed enter
                switch(selection)
                {
                    case 0: //saves data if yes is selected
                        if(saveData()==1)
                        {
                            //saving is successful, program exits
                            system("cls");
                            printf("\nSave successful, have a great day!\n"); //good bye message
                            return 1;
                        }
                        else
                        {
                            //saving is not successful, program reurns to title
                            system("cls");
                            printf("\nSave unsuccessful, press key to return...\n"); //error message
                            //wait for input
                            getch();
                        }
                        break;
                    case 2: //does not save if no is selected
                        system("cls");
                        printf("\nHave a great day!\n");
                        return 1;
                        break;
                }
                break;
        }
    }
    while(input!=27&&input!=8&&input!=13);

    //if the program has reached this point, it needs to return to the title (return 0)

    return 0;
}

/*
Function: displayCarPage
Does: Display pages of cars and manage user inputs (incorporates other functions based on user inputs: saveData, sortData, loadData, possibly others)
Uses:

- input (char): stores user's input for repeated use

- selectionCarNum (int): stores what car number the user's cursor is at on the page (used with pageNum to determine the selected car's number)

- i (int): miscellaneous counter, mainly used for printing options

- pageNum (int): used to track which page the user is on for convenient use

Receives: nothing
Returns: nothing
*/
void displayCarPages(void)
{
    //variables
    int selectedCarNum=0;
    int pageNum=0;
    int i;
    char input;

    //loops while input is neither esc or backspace (exit)
    do
    {
        system("cls");
        //if there are cars, display the cars
        if(numCars!=0)
        {
            for(i=0;i<5;i++)//five spaces are either empty or have cars
            {
                if(i<numCars-(pageNum*5))//find if given space is empty with numCars and pageNum
                {
                    if(i==selectedCarNum)// if the car is selected, draw little arrow thingies
                    {
                        printf("========================================\n");
                        printf("|Make:  %-31s| <\n|Model: %-31s| <\n|Owner: %-31s| <\n|       %-31s| <\n|Year: %-4i   $%-7i %-7iMiles    | <\n",carList[pageNum*5+i].make,carList[pageNum*5+i].model,carList[pageNum*5+i].ownerFName,carList[pageNum*5+i].ownerLName,carList[pageNum*5+i].year,carList[pageNum*5+i].initialPriceDollars,carList[pageNum*5+i].mileage);
                        printf("========================================\n");
                    }
                    else// otherwise draw it normally
                    {
                        printf("========================================\n");
                        printf("|Make:  %-31s|\n|Model: %-31s|\n|Owner: %-31s|\n|       %-31s|\n|Year: %-4i   $%-7i %-7iMiles    |\n",carList[pageNum*5+i].make,carList[pageNum*5+i].model,carList[pageNum*5+i].ownerFName,carList[pageNum*5+i].ownerLName,carList[pageNum*5+i].year,carList[pageNum*5+i].initialPriceDollars,carList[pageNum*5+i].mileage);
                        printf("========================================\n");
                    }
                }
                else //blank space if there is no car
                {
                    printf("\n\n\n\n\n\n");
                }
            }
        }
        //if there are no cars, says there are no cars but you can create some
        else
        {
            printf("Sorry, no cars to display.\n");
        }
        //display controls
        printf("\nPage %i/%i\n\nUp/Down - Select Car\nRight/Left - Select Page\nE - Sorting Menu\nS - Save\nEsc/Back - Exit",pageNum+1,(numCars-1)/5+1);
        input=getch();
        switch(input)
        {
            //all the arrow keys correspond to some capital letter in ascii for some reason, doesn't matter too much unless caps lock is on
            case 77://right arrow
                if(pageNum<(numCars-1)/5)
                {
                    pageNum++;//flip page
                    selectedCarNum=0;//reset cursor
                }
                break;
            case 75://left arrow
                if(pageNum>0)
                {
                    pageNum--;//flip page
                    selectedCarNum=0;//reset cursor
                }
                break;
            case 72://up arrow
                if(selectedCarNum>0)//move cursor
                {
                    selectedCarNum--;
                }
                break;
            case 80://down arrow
                if(selectedCarNum<numCars-(pageNum*5)-2)//move cursor
                {
                    selectedCarNum++;
                }
                break;
            case 'e':case 'E':
                displaySortingMenu();//call sorting function if E is pressed
                pageNum=0;//reset page for error-freeness
                selectedCarNum=0;//reset cursor for error-freeness
                break;
            case 's':case 'S':
                // S pressed, save data and display whether or not it was successful
                if(saveData()==1)
                {
                    system("cls");
                    printf("\nSave successful!\n");
                    getch();
                }
                else
                {
                    system("cls");
                    printf("\nSave unsuccessful...\n");
                    getch();
                }
                break;
        }
    }
    while(input!=27&&input!=8);
}

/*
Function: editCar
Does: Allows the user to edit properties of a given car
Uses:

- options (char[][]): a string array of all the options available to user, simplifies printing to a for loop

- input (char): stores user's input for repeated use

- selection (int): stores the location of the user's cursor

- i (int): miscellaneous counter, mainly used for printing options

Receives: integer number of the car the user is editing (i could give it a pointer but the carList is global so its more efficient to just give the number)
Returns: nothing
*/
void editCar(int carNum)
{
    char options[7][20]={"Make","Model","Name of Owner","Last Name of Owner","Year","Initial Price","Mileage"};
    int i;
    char input;
    char selection=0;

    if(numCars==0) //exit immediately if there are no cars
    {
        return;
    }

    do//while not esc or backspace
    {
        /* *************************
        largely the same old schtick with clearing, displaying options, getting input, using input, moving cursor
        commenting is less intense
        ****************************/
        system("cls");
        //car info printed
        printf("========================================\n");
        printf("|Make:  %-31s|\n|Model: %-31s|\n|Owner: %-31s|\n|       %-31s|\n|Year: %-4i   $%-7i %-7iMiles    |\n",carList[carNum].make,carList[carNum].model,carList[carNum].ownerFName,carList[carNum].ownerLName,carList[carNum].year,carList[carNum].initialPriceDollars,carList[carNum].mileage);
        printf("========================================\n");
        printf("\nYou have selected car #%i to edit, please select a property to edit:\n\n",carNum+1);
        for(i=0;i<7;i++)
        {
            if(selection==i)
            {
                printf(" > ");
            }
            else
            {
                printf("   ");
            }
            printf("%s\n",options[i]);
        }
        input=getch();
        switch(input)
        {
            case 72:
                if(selection>0)
                {
                    selection--;
                }
                break;
            case 80:
                if(selection<6)
                {
                    selection++;
                }
                break;
            case 13:
                //same car info reprinted
                system("cls");
                printf("========================================\n");
                printf("|Make:  %-31s|\n|Model: %-31s|\n|Owner: %-31s|\n|       %-31s|\n|Year: %-4i   $%-7i %-7iMiles    |\n",carList[carNum].make,carList[carNum].model,carList[carNum].ownerFName,carList[carNum].ownerLName,carList[carNum].year,carList[carNum].initialPriceDollars,carList[carNum].mileage);
                printf("========================================\n");
                switch(selection)
                {
                case 0:
                    //asks for input specific to case
                    printf("Please input the new make of car #%i\n\n",carNum+1);
                    //takes input specific to case
                    scanf("%30s",carList[carNum].make); //spaces can cause confusion but they shouldn't be there anyways, getch() would take too long to design

                    //do that x5
                    //vvvvvvvvv

                    break;
                case 1:
                    printf("Please input the new model of car #%i\n\n",carNum+1);
                    scanf("%30s",carList[carNum].model);
                    break;
                case 2:
                    printf("Please input the new name of the owner of car #%i\n\n",carNum+1);
                    scanf("%30s",carList[carNum].ownerFName);
                    break;
                case 3:
                    printf("Please input the new last name of the owner of car #%i\n\n",carNum+1);
                    scanf("%30s",carList[carNum].ownerLName);
                    break;
                case 4:
                    printf("Please input the new year of car #%i\n\n",carNum+1);
                    scanf("%i",&carList[carNum].year); //integer scanf riddled with errors, getch() would take too long to design
                    break;
                case 5:
                    printf("Please input the new initial price of car #%i\n\n",carNum+1);
                    scanf("%i",&carList[carNum].initialPriceDollars);
                    break;
                case 6:
                    printf("Please input the new mileage of car #%i\n\n",carNum+1);
                    scanf("%i",&carList[carNum].mileage);
                    break;
                }
                break;
        }
    }
    while(input!=27&&input!=8);
}

/*
Function: removeCar
Does: Allows the user to delete a given car from the list
Uses:

- options (char[][]): a string array of all the options available to user, simplifies printing to a for loop

- input (char): stores user's input for repeated use

- selection (int): stores the location of the user's cursor

- i (int): miscellaneous counter, mainly used for printing options

Receives: integer number of the car the user is removing (i could give it a pointer but the carList is global so its more efficient to just give the number)
Returns: nothing
*/
void removeCar(int carNum)
{
    int i;
    char options[2][20]={"Do Not Delete","Delete"};
    int selection=0;
    char input;

    if(numCars==0)//scientists have not yet discovered negative cars
    {
        return;
    }

    /* *************************
        largely the same old schtick with clearing, displaying options, getting input, using input, moving cursor
        commenting is less intense
        ****************************/

    do//while not esc, backspace, or enter
    {
        system("cls");
        printf("========================================\n");
        printf("|Make:  %-31s|\n|Model: %-31s|\n|Owner: %-31s|\n|       %-31s|\n|Year: %-4i   $%-7i %-7iMiles    |\n",carList[carNum].make,carList[carNum].model,carList[carNum].ownerFName,carList[carNum].ownerLName,carList[carNum].year,carList[carNum].initialPriceDollars,carList[carNum].mileage);
        printf("========================================\n");
        printf("\nYou have selected car #%i to be deleted, please confirm:\n\n",carNum+1);
        for(i=0;i<2;i++)
        {
            if(selection==i)
            {
                printf(" > ");
            }
            else
            {
                printf("   ");
            }
            printf("%s\n",options[i]);
        }
        input=getch();
        switch(input)
        {
            case 72:
                if(selection>0)
                {
                    selection--;
                }
                break;
            case 80:
                if(selection<1)
                {
                    selection++;
                }
                break;
            case 13:
                if(selection==1)//if you picked yes
                {
                    //swap with next car so that the car is at the back of the list and decrease the number of cars by 1
                    //the ca slot is now """empty""" and can be filled whenever a new car is created or just overwritten in loading the file
                    for(i=carNum;i<numCars;i++)
                    {
                        switchCars(&carList[i],&carList[i+1]);
                    }
                    numCars--;
                }
                break;
        }
    }
    while(input!=27&&input!=8&&input!=13);
}

/*
Function: createCar
Does: Allows the user to create a new car, given that the number of cars is not currently 100 (max)
Uses:



Receives: nothing
Returns: nothing
*/
void createCar(void)
{
    if(numCars==100)
    {
        system("cls");
        printf("\nDatabase is full, no new cars may be entered...\n");
        getch();
        return;
    }
}

/*
Function: loadData
Does: Loads in all the data contained in the "cars.txt" file in the order that it is in
Uses:

- input (char): stores user's input for repeated use

Receives: nothing
Returns: integer 1 or 0 based on if it was successful or not
*/
int loadData(void)
{
    //variables
    char input;

    numCars=0;//resets the number of cars to be incremented from 0

    FILE*fp=fopen("cars.txt","r");//opens file in read mode (literally no need for r+, its just reading)

    /* ****************
    Big fat block of code that boils down to:

    hey the file didn't open, there's something wrong with it

    wanna make a new one? >yes  >didnt work, exiting
                                >worked please restart and make some cars

                          >no   >ok exiting
    ******************/
    if(fp==NULL)
    {
        system("cls");
        printf("\n\ncars.txt is missing or cannot be accessed, attempt creating it? [y/n]\n\n");
        do
        {
            input=getch();
            switch(input) // using y and n, no cursor cause' I'm lazy and this will rarely happen
            {
                case 'y':case 'Y':
                    fp=fopen("cars.txt","w"); //no need for anything fancier than w
                    if(fp==NULL)
                    {
                        printf("WOW, EVEN THAT DIDN'T WORK\nQuitting program, have a nice day!");
                    }
                    else
                    {
                        printf("File created, please restart the program, exiting...");
                    }
                    fclose(fp); //closing
                    getch();
                    return 0;
                    break;
                case 'n':case 'N':
                    printf("OK, Quiting program, have a nice day!");
                    getch();
                    return 0;
                    break;
            }
        }
        while(input!=78&&input!=89&&input!=110&&input!=121);//all the letters in ascii
        return 0;
    }

    //actually loading the data!!!!!!!!!!

    while(!feof(fp))//while not the end of the file
    {
        fscanf(fp,"%s",carList[numCars].make); //scans a string into the property, pretty simple
        //if statement catches cars with missing info and also makes it so 0 cars is actually registered as 0 cars because feof() is stupid
        if(carList[numCars].make[0]==0)
        {
            break;
        }
        fscanf(fp,"%s",carList[numCars].model);//repeat
        if(carList[numCars].model[0]==0)
        {
            break;
        }
        fscanf(fp,"%s",carList[numCars].ownerFName);//repeat
        if(carList[numCars].ownerFName[0]==0)
        {
            break;
        }
        fscanf(fp,"%s",carList[numCars].ownerLName);//repeat
        if(carList[numCars].ownerLName[0]==0)
        {
            break;
        }
        //no if statements for the integers, these lil critters are hard to catch and designing fgetc() would take too much time
        fscanf(fp,"%i",&carList[numCars].year);
        fscanf(fp,"%i",&carList[numCars].initialPriceDollars);
        fscanf(fp,"%i",&carList[numCars].mileage);
        //Made separate lines for formatting purposes
        numCars++;
        //numCars used as a counter because it can be
    }

    fclose(fp);//closing file

    //returns 1 if everythig is successful and it made it here

    return 1;
}

/*
Function: saveData
Does: Rewrites the "cars.txt" file to contain the data which is currently loaded how it is sorted in that moment
Uses:

- i (int): counter to keep track of car number

Receives: nothing
Returns: integer 1 or 0 based on if it was successful or not
*/
int saveData(void)
{
    int i;

    FILE*fp=fopen("cars.txt","w");//open in rewrite mode
    if(fp==NULL)//if it didn't open, return 0 and leave
    {
        return 0;
    }

    for(i=0;i<numCars;i++)//from 0 to how many cars - 1
    {
        //print all the properties on separate lines back to file
        fprintf(fp,"%s\n",carList[i].make);
        fprintf(fp,"%s\n",carList[i].model);
        fprintf(fp,"%s\n",carList[i].ownerFName);
        fprintf(fp,"%s\n",carList[i].ownerLName);
        fprintf(fp,"%i\n",carList[i].year);
        fprintf(fp,"%i\n",carList[i].initialPriceDollars);
        fprintf(fp,"%i\n",carList[i].mileage);
        //Made separate lines for formatting purposes
    }

    fclose(fp); //close

    //success!

    return 1;
}

/*
Function: sortData
Does: Sorts data using bubble sort based on a specified parameter (see "car" structure declaration), only sorts based on one parameter (if 2 cars have an element with the same value, their order relative to each other does not change)
Uses:

a bunch of random counters: i and j are for bubble sort, k is for looking for different letters in strings

Receives: an integer, type, representing the type of sorting (1-7, see "car" structure declaration)
Returns: nothing
*/
void sortData(int type)
{
    //counters
    int i,j,k=0;

    for(i=0;i<numCars;i++) //goes for how many elements need to be sorted i.e. 0 to numCars-1 (bubble sort)
    {
        for(j=0;j<numCars-1;j++) //checks every pair of elements
        {
            switch(type) //does things based on the given type of sorting (a lot of copypaste)
            {
                case 1:case 2://make
                    k=0;//just in case
                    while(toupper(carList[j].make[k])==toupper(carList[j+1].make[k]))//loop breaks if it finds two different letters or it reaches the end
                    {
                        k++;
                        if(k==31)
                        {
                            break;
                        }
                    }
                    if(type==1)//determines alpha letter
                    {
                        if(toupper(carList[j].make[k])>toupper(carList[j+1].make[k]))//toupper used
                        {
                            switchCars(&carList[j],&carList[j+1]);//switches if the last one is more alpha
                        }
                    }
                    else //determines reverse alpha letter
                    {
                        if(toupper(carList[j].make[k])<toupper(carList[j+1].make[k]))
                        {
                            switchCars(&carList[j],&carList[j+1]);//switches if the last one is less alpha
                        }
                    }
                    break;
                case 3:case 4://model (literally copypaste and change name and ome numbers, no new comments)
                    k=0;//just in case
                    while(toupper(carList[j].model[k])==toupper(carList[j+1].model[k]))
                    {
                        k++;
                        if(k==31)
                        {
                            break;
                        }
                    }
                    if(type==3)
                    {
                        if(toupper(carList[j].model[k])>toupper(carList[j+1].model[k]))
                        {
                            switchCars(&carList[j],&carList[j+1]);
                        }
                    }
                    else
                    {
                        if(toupper(carList[j].model[k])<toupper(carList[j+1].model[k]))
                        {
                            switchCars(&carList[j],&carList[j+1]);
                        }
                    }
                    break;
                case 5:case 6://year (same as alphabet sorting but easier because you can just compare them right away)
                    if(type==5&&carList[j].year<carList[j+1].year)
                    {
                        switchCars(&carList[j],&carList[j+1]);
                    }
                    else if(type==6&&carList[j].year>carList[j+1].year)
                    {
                        switchCars(&carList[j],&carList[j+1]);
                    }
                    break;
                case 7:case 8://price (same)
                    if(type==7&&carList[j].initialPriceDollars<carList[j+1].initialPriceDollars)
                    {
                        switchCars(&carList[j],&carList[j+1]);
                    }
                    else if(type==8&&carList[j].initialPriceDollars>carList[j+1].initialPriceDollars)
                    {
                        switchCars(&carList[j],&carList[j+1]);
                    }
                    break;
                case 9:case 10://mileage (same)
                    if(type==9&&carList[j].mileage>carList[j+1].mileage)
                    {
                        switchCars(&carList[j],&carList[j+1]);
                    }
                    else if(type==10&&carList[j].mileage<carList[j+1].mileage)
                    {
                        switchCars(&carList[j],&carList[j+1]);
                    }
                    break;
            }
        }
    }
}

/*
Function: displaySortingMenu
Does: lets the user choose how to sort and does so then returns
Uses:

- options (char[][]): a string array of all the options available to user, simplifies printing to a for loop

- input (char): stores user's input for repeated use

- selection (int): stores the location of the user's cursor

- i (int): miscellaneous counter, mainly used for printing options

Receives: nothing
Returns: nothing
*/
void displaySortingMenu(void)
{
    char options[10][50]={"Make (Aplhabetical)","Make (Rev. Aplhabetical)","Model (Aplhabetical)","Model (Rev. Aplhabetical)","Year (Newest)","Year (Oldest)","Price (Highest)","Price (Lowest)","Mileage (Lowest)","Mileage (Highest)"};
    char input;
    int i;
    int selection=0;

    /* *************************
        largely the same old schtick with clearing, displaying options, getting input, using input, moving cursor
        commenting is less intense
        ****************************/

    do//until esc backspace or enter
    {
        system("cls");
        printf("=======\nSorting\n=======\n\nPlease select a property to sort by:\n\n");
        for(i=0;i<10;i++)
        {
            if(i==selection)
            {
                printf(" > ");
            }
            else
            {
                printf("   ");
            }
            printf("%s\n",options[i]);
        }
        printf("\nEnter - Confirm\nEsc - Exit");
        input=getch();
        switch(input)
        {
        case 72:
            if(selection>0)
            {
                selection--;
            }
            break;
        case 80:
            if(selection<9)
            {
                selection++;
            }
            break;
        case 13:
            sortData(selection+1); //pass the user's selection+1 to the sortData function, the loop will break since the user pressed enter
            break;
        }
    }
    while(input!=27&&input!=8&&input!=13);
}

/*
Function: switchCars
Does: Switched two car structures, useful for sorting and proving that I can use pointers ;)
Uses: car structure, tempCar, used as a temporary variable for swapping
Receives: pointers to two car structures to be swapped
Returns: nothing
*/
void switchCars(car*carA,car*carB)
{
    car tempCar;

    tempCar=*carA;
    *carA=*carB;
    *carB=tempCar;
}
