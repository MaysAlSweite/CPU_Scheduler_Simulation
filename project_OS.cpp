#include <iostream>
#include <queue>
#include <iomanip>
#include<string.h>
#include <fstream>
using namespace std;

int procNo,CS,Q;

struct PCB
{
    string id;
    int AT;
    int BT;
    int ST;//wt=st-at
    int WT;
    int FT;//ft=st+bt
    int TAT;
    int RT;
    bool done;
};
// Function to read process data from a text file
std::vector<PCB> readProcesses( string filename)
{

    std::vector<PCB> processes;
    ifstream fin(filename);

    if (!fin.is_open())
    {
        std::cerr << "Could not open file: " << filename << '\n';
    }
    else
    {
        PCB process;
        fin>>procNo>>Q>>CS;
        while (fin >> process.id >> process.AT >> process.BT)
        {
            processes.push_back(process);

        }

        fin.close();
        return processes;
    }
}
void Print(vector<PCB> &processes)
{

    for(int i=0; i<procNo; i++)
    {
        cout<<processes[i].id<<"\t"<<"Start  " <<processes[i].ST<< "\t"<<"  Waiting "<<processes[i].WT<<"\t"<<" Finish  "
            <<processes[i].FT<<"\t"<<"Turnaround    "<<processes[i].TAT<<endl;
    }
}



int Initialization(vector<PCB> &processes)
{

    int  burstTotal=0;
    for(int i=0; i<procNo; i++)
    {
        processes[i].RT=processes[i].BT;
        processes[i].done=0;
        processes[i].WT=0;
        processes[i].TAT=0;
        burstTotal+=processes[i].BT;
    }
    return burstTotal;
}

void cpuUtilize(int tot, int burstTotal)
{
    double total=static_cast<double>(tot);
    double cpu=static_cast<double>(total/burstTotal);
    cpu *=100.0;
    cout<<"Ucpu = "<<total<<"/"<< burstTotal<<"="<< std::fixed << std::setprecision(2)<<cpu<<"%\n"<<endl;
}

void FCFS( vector<PCB> &processes,int burstTotal)
{
    int total=burstTotal;
    int currentTime = 0;
    cout<<" FCFS Gantt Chart\n ";
    for (int i = 0; i < procNo; ++i)
    {
        PCB& process = processes[i];
        process.ST = currentTime;
        cout<<"("<<currentTime<<") |==P"<<i<<"==| ";
        currentTime += process.BT;
        process.FT = currentTime;
        if(CS>0 && currentTime<burstTotal)
        {
            cout<<"("<<currentTime<<") |==CS==| ";
            burstTotal+=CS;
        }
        process.TAT = process.FT - process.AT;
        process.WT = process.ST - process.AT;
        currentTime += CS;

    }
    cout<<"("<<burstTotal<<")"<<endl;
    Print(processes);
    cpuUtilize(total,burstTotal);
}
//returns index of least RT arrived process
int getNextProcess(vector<PCB> &processes,int time)
{
    int i,index;
    for(i=0; i<procNo; i++)
    {
        if(processes[i].done==0)
            index=i;
        break;
    }

    for(i=0; i<procNo; i++)
    {
        if(processes[i].done==0)
            if(processes[i].RT<processes[index].RT && processes[i].AT<=time)
                index=i;
    }

    return index;

}


void SRT(vector<PCB> &processes,int burstTotal)
{
    int total=burstTotal;
    int next=0,old=0;
    cout<<"Gantt Chart\n ";
    for(int j=0; j<=burstTotal; j++ ) //
    {
        //j is time

        next=getNextProcess(processes,j);//returns index of p that notReady+Arrived+less RT

        if(old!=next || j==0)
        {
            if(CS>0 && j>0)
            {
                cout<<"("<<j<<")|==CS==|";
                j+=CS;
                burstTotal+=CS;
            }

            cout<<"("<<j<<")|==P"<<next<<"==|";


            processes[next].ST=j;

        }

        processes[next].RT-=1;

        if(processes[next].RT==0)
        {
            processes[next].FT=j+1;
            processes[next].TAT=processes[next].FT-processes[next].AT;
            processes[next].done=1;
        }


        for(int i=0; i<procNo; i++)
            if(i!=next && processes[i].done==0 && processes[i].AT<=j)
                processes[i].WT+=1;

    }
    cout<<"("<<burstTotal<<")"<<endl;
    old=next;
    Print(processes);
    cpuUtilize(total,burstTotal);
}



void RoundRobin(vector<PCB> &processes,int burstTotal)
{
    int total=burstTotal;
    int currentTime = 0;
    int lastProcess = -1;
    int completedProcesses = 0;

    for (int i = 0; i < procNo; i++)
    {
        processes[i].ST=-1;
    }

    while (completedProcesses < procNo)
    {
        bool foundProcess = false;

        for (int i = 0; i < procNo; i++)
        {
            int index = (lastProcess + 1 + i) % procNo;

            if (processes[index].RT > 0 && processes[index].AT <= currentTime)   //واصل ومش جاهز
            {
                if (processes[index].ST == -1)
                {
                    processes[index].ST = currentTime;

                }

                cout <<"("<<currentTime<<")" "|==P"<<index<<"==|";

                foundProcess = true;
                int quantamtime;
                if (Q < processes[index].RT)
                {
                    quantamtime = Q;
                }
                else
                {
                    quantamtime = processes[index].RT;
                }

                processes[index].RT -= quantamtime;
                currentTime += quantamtime;

                if (processes[index].RT == 0)
                {
                    processes[index].FT = currentTime;
                    processes[index].TAT = currentTime - processes[index].AT;
                    processes[index].WT = processes[index].TAT - processes[index].BT;
                    completedProcesses++;

                }

                lastProcess = index;
                currentTime += CS;
                if(CS>0 )
                {
                    if(i==0)
                    {
                        cout <<"("<<currentTime-CS<<")";
                        cout<<"|==CS==|";
                    }
                    else
                    {
                        if(currentTime<=burstTotal)
                            cout<<"("<<currentTime<<")"<<"|==CS==|";
                    }
                }
                burstTotal+=CS;

                break;
            }
        }

        if (!foundProcess)
        {
            currentTime++;
        }
    }
    cout<<"("<<burstTotal<<")"<<endl;
    Print(processes);
    cpuUtilize(total,burstTotal);
}
int main()
{
    std::vector<PCB> processes=readProcesses("processes.txt.txt");
    int burstTotal=Initialization(processes);
    int choice;
    cout<<"Enter your choice \n 1.FCFS \n 2.SRT \n 3.Round Robin "<<endl;
    cin>>choice;
    switch(choice)
    {
    case 1:
        FCFS(processes,burstTotal);
        break;
    case 2 :
        SRT(processes,burstTotal);
        break;
    case 3 :
        RoundRobin(processes,burstTotal);
        break;
    default:
        cout<<"Please enter valid choice"<<endl;
        break;
    }

    system("pause");
    return 0;
}
