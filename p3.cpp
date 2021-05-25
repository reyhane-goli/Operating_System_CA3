#include <iostream>
#include <semaphore.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <thread> 
#include <time.h>
#include <ctime>
#include <chrono>
#include <math.h>
#include <stdio.h>
using namespace std;
using namespace std::chrono;

class Monitor
 {
   public:
        void ConditionWait();        
        void ConditionSignal();
        void acquire();
        void release();
        Monitor();  
   private:         
   sem_t cvar;           
   int waiters;               
   sem_t lock;            
   sem_t next;           
   int nextCount;          
   bool busy; 
 }; 
                           
 Monitor::Monitor() 
 {    
 sem_init(&cvar, 0, 0);;       
 sem_init(&lock, 0, 1);;     
 sem_init(&next, 0, 0); 
 nextCount = waiters = 0;
 busy=false;
 }

 void Monitor::ConditionWait() 
 {                
  waiters += 1;    
  if (nextCount > 0)        
    sem_post(&next);       
  else         
    sem_post(&lock);    
  sem_wait(&cvar);           
  waiters -= 1;   
 } 

 void Monitor::ConditionSignal()
 {               
  if (waiters > 0) 
  {         
  nextCount += 1;
  sem_post(&cvar);              
  sem_wait(&next);             
  nextCount -= 1;
  }     
 }

void Monitor::acquire() 
{ 
  sem_wait(&lock);

  if (busy)
    ConditionWait();                                                                                                   
  busy = true;

  if (nextCount > 0) 
    sem_post(&next); 
  else 
    sem_post(&lock); 
}

void Monitor::release() 
{ 
  sem_wait(&lock);

  busy = false; 
  ConditionSignal();

  if (nextCount > 0) 
    sem_post(&next); 
  else 
    sem_post(&lock);

} 

vector<string>roadwith_h;
vector<string>road;
vector<int>h;
vector<string>path;
vector<vector<string>>part_of_path;
vector<string>num_of_car;
vector<thread>threads;
vector<Monitor>monitors;
vector<vector<int>>pollution;

void T_function(int n,int car_num)
{
   vector<string>out;
   for(int i=0;i<part_of_path[n].size();i++)
   {
    for(int j=0;j<road.size();j++)
    {
      if(part_of_path[n][i]==road[j])
      {
        monitors[j].acquire();
        milliseconds enterance_time=duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        int hardness;
        int p_rand;
        int part_of_path_pollution=0;
        hardness=h[j];
        srand (time(NULL));
        p_rand=rand() % 10+1;
        long double emission=0.0;
        for(int k=0;k<=10000000;k++)
        {
          emission=emission+ceil(k*1.0/(1000000*p_rand*hardness));
          //part_of_path_pollution=part_of_path_pollution+ceil(k*1.0/(1000000*p_rand*hardness));
          //cout<<"part_of_path_pollution is "<<part_of_path_pollution<<"\n";
        }
        part_of_path_pollution=(int)emission;
        pollution[n].push_back(part_of_path_pollution);
        monitors[j].release();
        milliseconds exit_time=duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        int total=0;
        for(int i=0;i<pollution[n].size();i++)
        {
          total=total+pollution[n][i];

        }
        /* cout<<((part_of_path[n])[i])[0]<<", "<<enterance_time.count()<<", "<<((part_of_path[n])[i])[4]<<", "<<exit_time.count()<<", "<<
              pollution[n][i]<<", "<<total<<"\n";*/
        string a(1,((part_of_path[n])[i])[0]);
        string b(1,((part_of_path[n])[i])[4]);
        string s=a+", "+to_string(enterance_time.count())+", "+b+", "+to_string(exit_time.count())+", "+
                  to_string(pollution[n][i])+", "+to_string(total);
        out.push_back(s);
        break;
      }
    }
   }

  string out_file_name;
  out_file_name="<"+to_string(n)+">-<"+to_string(car_num)+">.txt";
  ofstream file(out_file_name, ios::out );
  for(int i=0;i<out.size();i++)
  {
    file << out[i] <<"\n";
  }     
  file.close();
}

int main(int argc,char* argv[])
 {
   int p;
   int sum;
   int flag=0;
   int count=0;
   string myText;
   string filename;
   Monitor monitor_p;
   cout<<"please enter the filename"<<"\n";
   cin>>filename;
   ifstream MyReadFile(filename);
   while (getline (MyReadFile, myText)) 
   {
     if(myText!="#" && flag==0)
     {
     	roadwith_h.push_back(myText);
	   }
	   if(myText=="#")
	   {
        flag=1;
	   }
     if(myText!="#" && flag==1)
     {
      count++;
      if(count%2==0)
      {
        num_of_car.push_back(myText);
      }
      if(count%2!=0)
      {
        path.push_back(myText);
      }
     }
   }
  MyReadFile.close();

  for(int i=0;i<roadwith_h.size();i++)
  {
    string s;
    int H;
    s.append(roadwith_h[i],0,5);
    road.push_back(s);
    H=(roadwith_h[i])[8]-'0';
    h.push_back(H);
  }

  for(int i=0;i<path.size();i++)
  {
    part_of_path.push_back(vector<string>());
    for(int j=0;j<path[i].size()-1;j=j+4)
    {
      string str;
      str.append(path[i],j,5);
      part_of_path[i].push_back(str);

    }
  }

  for(int i=0;i<path.size();i++)
  {
    pollution.push_back(vector<int>());
  }

  for(int i=0;i<road.size();i++)
  {
    monitors.push_back(monitor_p);
  }

  for(int i=0;i<path.size();i++)
  {
    for(int j=0;j<((num_of_car[i])[0])-'0';j++)
    {
      threads.push_back(thread(T_function,i,j+1));
    }
  }

  for(int i=0;i<threads.size();i++)
  {
    threads[i].join();
  }

 }