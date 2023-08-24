//PUT prototypes above and defs below main
#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#define limit 3
using namespace std;

//Graph implementation---------
class Graph
{
private:
    //vector implementation (adjacency list)
    vector<int> *adjacencylist;
    int size;
public:
    Graph(){};
    vector<int>* getGraph();
    int getSize();
    void insertNodes(int);
    //void deleteNodes(int);//deletes given number of nodes
    void insertEdge(int,int);
    //void deleteEdges(int);//deletes all edges of given node
    //~Graph(){delete [] adjacencylist;};
};
vector<int>* Graph::getGraph()
{
    return adjacencylist;
}
int Graph::getSize()
{
    return size;
}
void Graph::insertNodes(int s)
{
    size=s;
    adjacencylist=new vector<int>[size];
}
// void Graph::deleteNodes(int value)
// {
//     for(int i=value;i>value;i--)
//     adjacencylist[i].clear();
// }
void Graph::insertEdge(int node,int node2)//uni-directional edge
{
    adjacencylist[node].push_back(node2);
}
//check this function
// void Graph::deleteEdges(int value)
// {
//     adjacencylist[value].clear();
// }
//Graph implementation End--------

//defining necessary structures
struct Pagerank//of a node or website
{
    float value;
    float outgoing_links;
    vector<int> nodes_pointing;
};
struct CTR
{
    float value;
    int clicks;
    int impressions;
};
//for storing data of website
struct Website
{
    int node;
    float score;
    string url;
    vector<string> keywordlist;
    CTR ctr;
    Pagerank pagerank;
};

//function prototypes
vector<Website> readfromFile(vector<Website> weblist);
Graph makeWebgraph(vector<Website> weblist);
vector<Website> Search(vector<Website> weblist,int type,string search_query);
vector<Website> assignPagerank(vector<Website> weblist,Graph webgraph);
vector<Website> calculateCTR(vector<Website> weblist);
//for sorting
void swap(Website* obj1,Website* obj2);
vector<Website> pagerankSort(vector<Website> pr_list, int n);
vector<Website> scoreSort(vector<Website> ctr_list, int n);
vector<Website> sortResults(vector<Website> result_list);
//for updating values
vector<Website> updateImpressions(vector<Website> weblist,vector<Website> result_list);
void updateFile(vector<Website> weblist);
//for printing list
void printList(vector<Website> result_list);

int main()
{
    vector<Website> weblist;
    weblist=readfromFile(weblist);
    Graph webgraph;
    webgraph=makeWebgraph(weblist);//webgraph made
    weblist=assignPagerank(weblist,webgraph);//assign pagerank using webgraph
    weblist=calculateCTR(weblist);
    sortResults(weblist);
    //variables used
    int menu_choice,search_choice,search_num,choice;string search_query;
    vector<Website> result_list;
    //display menu
    cout<<"Welcome!";
    cout<<"What would you like to do:\n1.New Search\n2.Exit\n\n";
    cout<<"Your choice: ";cin>>menu_choice;
    switch(menu_choice)
    {
        case 1:
        search:
        cout<<"Search:";
        cin.ignore();
        getline(cin,search_query);
        //formatting the search query
        if(!search_query.find('"'))//if quotes are present
        {
            search_query=search_query.substr(1,search_query.length()-2);//remove quotes
            result_list=Search(weblist,1,search_query);
        }
        else if(search_query.find("AND")<search_query.length())//AND is present
        {
            result_list=Search(weblist,2,search_query);
        }
        else if(search_query.find("OR")<search_query.length())//OR is present
        {
            result_list=Search(weblist,3,search_query);
        }
        else//simple search query multiple words
        {
            result_list=Search(weblist,4,search_query);
        }
        result_list=sortResults(result_list);
        weblist=updateImpressions(weblist,result_list);
        searchresults:
        printList(result_list);
        cout<<"Would you like to:\n1.Choose a webpage to open\n2.New Search\n3.Exit\n\n";
        cout<<"Your choice: ";cin>>search_choice;
        switch (search_choice)
        {
        case 1:
            cout<<"Enter webpage number:";cin>>search_num;
            cout<<"You're now viewing "<<result_list[search_num-1].url<<endl;
            weblist[result_list[search_num-1].node].ctr.clicks++;
            cout<<"Would you like to:\n1.Back to search results\n2.New Search\n3.Exit\n\n";
            cout<<"Your choice: ";cin>>choice;
            if(choice==1)
            goto searchresults;
            else if(choice==2)
            goto search;
            else if(choice==3)
            {
                updateFile(weblist);
                exit(1);
            }
            break;
        case 2:
            goto search;
        case 3:
            updateFile(weblist);
            exit(1);
        default:
            break;
        }
        break;
        case 2:
        //update file before exitting
        updateFile(weblist);
        exit(1);
        default:
        cout<<"Invalid choice!\n";
    }
}
vector<Website> readfromFile(vector<Website> weblist)
{
    ifstream keywordfile,impressionsfile;
    keywordfile.open("keyword.csv");
    impressionsfile.open("impressions.csv");
    //file opening exception handling
    if(keywordfile.is_open()&&impressionsfile.is_open())
    cout<<"files opened successfully!\n";
    else
    cout<<"file not open!\n";
    cout<<"fetching data...\n";
    string line,word;
    int counter=0;//for assignning node number to each website
    //read website url and store keywords of it and make weblist
    while(!keywordfile.eof())
    {
        Website webobj;
        webobj.node=counter;
        getline(keywordfile,line);
        stringstream linestream(line);
        getline(linestream,word,',');
        webobj.url=word;
        while(getline(linestream,word,','))
        webobj.keywordlist.push_back(word);
        webobj.ctr.clicks=0;
        weblist.push_back(webobj);
        counter++;
    }
    //read impressions and store accordingly in weblist
    while(!impressionsfile.eof())
    {
        Website webobj;int impression;
        getline(impressionsfile,line);
        stringstream linestream(line);
        getline(linestream,word,',');
        webobj.url=word;//website url stored
        getline(linestream,word);
        stringstream number(word);
        number>>impression;//website impressions stored

        //iterate through weblist and store impressions accordingly
        for(int i=0;i<weblist.size();i++)//linear search
        {
            if(webobj.url==weblist[i].url)
            {
                weblist[i].ctr.impressions=impression;
            }
        }
    }
    keywordfile.close();
    impressionsfile.close();
    return weblist;
}
Graph makeWebgraph(vector<Website> weblist)
{
    Graph webgraph;
    webgraph.insertNodes(limit);
    ifstream webgraphfile;
    webgraphfile.open("webgraph.csv");
    if(webgraphfile.is_open())
    cout<<"file opened successfully!\n";
    else
    cout<<"file not open!\n";
    cout<<"fetching data...\n";
    string line,word;int node,node2;
    while(!webgraphfile.eof())
    {
        getline(webgraphfile,line);
        stringstream linestream(line);
        getline(linestream,word,',');
        for(int i=0;i<weblist.size();i++)//for 1st url
        {
            if(word==weblist[i].url)//linear search
            {
                node=i;
                break;
            }  
        }
        getline(linestream,word);
        for(int i=0;i<weblist.size();i++)//for 2nd url
        {
            if(word==weblist[i].url)//linear search
            {
                node2=i;
                break;
            }     
        }
        webgraph.insertEdge(node,node2);
    }
    webgraphfile.close();
    cout<<"Webgraph made!\n";
    return webgraph;
}
vector<Website> Search(vector<Website> weblist,int type,string search_query)
{
    //SEARCH IMPLEMENTATION(Linear Search)
    vector<Website> result_list;vector<bool> search;
    vector<string> word;//words of search stored in this
    if(type==1)//search with quotation marks
    {
        //SEARCH IMPLEMENTATION(Linear Search)
        for(int i=0;i<weblist.size();i++)//iterate websites
        {
            for(int j=0;j<weblist[i].keywordlist[j].size();j++)//iterate keywords of website
            {
                if(weblist[i].keywordlist[j]==search_query)//check if words match
                {
                    result_list.push_back(weblist[i]);
                    break;
                }
            }
        }
    }
    else if(type==2)//AND in query
    {
        int counter=0;string w;
        stringstream line(search_query);
        while(!line.eof())
        {
            counter++;
            if(counter%2==0)
            {
                getline(line,w,' ');
                continue;
            }
            getline(line,w,' ');
            word.push_back(w);
        }
        for(int i=0;i<weblist.size();i++)//iterate websites
        {
            for(int j=0;j<weblist[i].keywordlist[j].size();j++)//iterate keywords of website
            {
                for(int k=0;k<word.size();k++)//iterate words of search query
                {
                    if(weblist[i].keywordlist[j]==word[k])//check if words match
                    {
                        search.push_back(true);
                        break;
                    }
                }
            }
            if(search.size()==word.size())
            {
                result_list.push_back(weblist[i]);
                search.clear();
                break;
            }
        }
    }
    else if(type==3)//OR in query or simple query
    {
        int counter=0;string w;
        stringstream line(search_query);
        while(!line.eof())
        {
            counter++;
            if(counter%2==0)
            {
                getline(line,w,' ');
                continue;
            }
            getline(line,w,' ');
            word.push_back(w);
        }
        for(int i=0;i<weblist.size();i++)//iterate websites
        {
            for(int j=0;j<weblist[i].keywordlist[j].size();j++)//iterate keywords of website
            {
                for(int k=0;k<word.size();k++)//iterate keywords of search_query
                {
                    if(weblist[i].keywordlist[j]==word[k])//check if words match
                    {
                        search.push_back(true);
                        break;
                    }
                }
            }
            if(!search.empty())
            result_list.push_back(weblist[i]);
            search.clear();
        }
    }
    else if(type==4)//simple query
    {
        string w;
        stringstream line(search_query);
        while(!line.eof())
        {
            getline(line,w,' ');
            word.push_back(w);
        }
        for(int i=0;i<weblist.size();i++)//iterate websites
        {
            for(int j=0;j<weblist[i].keywordlist[j].size();j++)//iterate keywords of website
            {
                for(int k=0;k<word.size();k++)//iterate keywords of search_query
                {
                    if(weblist[i].keywordlist[j]==word[k])//check if words match
                    {
                        search.push_back(true);
                        break;
                    }
                }
            }
            if(!search.empty())
            result_list.push_back(weblist[i]);
            search.clear();
        }
    }
    return result_list;
}
vector<Website> assignPagerank(vector<Website> weblist,Graph webgraph)//----* 
{
    vector<int>* list;
    vector<float> prevpr;//previous pr of nodes
    float nodes;
    list=webgraph.getGraph();//stores graph
    nodes=webgraph.getSize();//total nodes/websites in graph
    //calculating pagerank
    //find outgoing links of all nodes, also initialize pageranks
    for(int a=0;a<weblist.size();a++)
    {
        weblist[a].pagerank.outgoing_links=list[a].size();
        weblist[a].pagerank.value=0;
        prevpr.push_back(1.0/nodes);
    }
    //calculate pointing nodes for each node
    for(int i=0;i<nodes;i++)//node to search
    {
        for(int j=0;j<nodes;j++)//iterating all nodes
        {
            for(int k=0;k<list[j].size();k++)//iterating list of each node
            {
                if(i==list[j][k])//match found
                weblist[i].pagerank.nodes_pointing.push_back(j);
            }
        }
    }
    for(int i=1;i<nodes-1;i++)//iterations
    {
        for(int j=0;j<nodes;j++)//nodes
        {
            for(int k=0;k<weblist[j].pagerank.nodes_pointing.size();k++)//pointing nodes list
            {
                int cn=weblist[j].pagerank.nodes_pointing[k];//current pointing node
                weblist[j].pagerank.value+=prevpr[cn]/weblist[cn].pagerank.outgoing_links;
            }
        }
        if(i!=nodes-2)
        {
            for(int k=0;k<nodes;k++)//weblist->prlist values copied
            {
                prevpr[k]=weblist[k].pagerank.value;
                weblist[k].pagerank.value=0;
            }
        }
    }
    return weblist;
}
vector<Website> calculateCTR(vector<Website> weblist)
{
    for(int i=0;i<weblist.size();i++)
    {
        weblist[i].ctr.value=weblist[i].ctr.clicks/weblist[i].ctr.impressions;
    }
    return weblist;
}
void swap(Website* obj1,Website* obj2)
{
    Website* temp;
    temp=obj1;
    obj1=obj2;
    obj2=temp;
}
vector<Website> scoreSort(vector<Website> ctr_list, int n) 
{ 
    //Using bubble sort
    int i, j; 
    for (i = 0; i < n-1; i++)     
    {
        for (j = 0; j < n-i-1; j++) 
        {
            if (ctr_list[j].score > ctr_list[j+1].score) 
            swap(&ctr_list[j], &ctr_list[j+1]); 
        }
    }
    return ctr_list;
} 
vector<Website> pagerankSort(vector<Website> pr_list, int n) 
{ 
    //Using bubble sort
    int i, j; 
    for (i = 0; i < n-1; i++)     
    {
        for (j = 0; j < n-i-1; j++) 
        {
            if (pr_list[j].pagerank.value > pr_list[j+1].pagerank.value) 
            swap(&pr_list[j], &pr_list[j+1]); 
        }
    }
    return pr_list;//containing sorted data
} 
vector<Website> sortResults(vector<Website> result_list)
{
    vector<Website> ctr_list;//sort records based on ctr(score)
    vector<Website> pr_list;//sort records based on rank
    vector<Website> sorted_list;
    for(int i=0;i<result_list.size();i++)//separate results
    {
        if(result_list[i].ctr.value!=0)
        ctr_list.push_back(result_list[i]);
        else
        pr_list.push_back(result_list[i]);
    }
    //if ctr=0, use pagerank
    if(pr_list.size()==result_list.size())
    {
        pr_list=pagerankSort(pr_list,pr_list.size());
        return pr_list;
    }
    //if ctr>0, use score
    //CALCULATE SCORE
    //pagerank normalized: normalized pagerank of all websites
    float prnorm=0;//pagerank normalized
    for(int i=0;i<result_list.size();i++)
    {
        prnorm+=pr_list[i].pagerank.value;
    }
    prnorm=prnorm/result_list.size();
    //update score for each website in results list
    for(int i=0;i<result_list.size();i++)
    {
        int impressions=pr_list[i].ctr.impressions;int ctr=ctr_list[i].ctr.value;
        int temp=(0.1*impressions)/(1+(0.1*impressions));
        ctr_list[i].score=(0.4*prnorm)+(1-temp)*((prnorm+temp)*ctr)*0.6;
    }
    //sorting and inserting back in result list
    //bubble sort
    pr_list=pagerankSort(pr_list,pr_list.size());
    ctr_list=scoreSort(ctr_list,ctr_list.size());
    for(int i=0;i<ctr_list.size();i++)
    {
        sorted_list.push_back(ctr_list[i]);
    }
    for(int i=0;i<ctr_list.size();i++)
    {
        sorted_list.push_back(pr_list[i]);
    }
    return sorted_list;
} 
vector<Website> updateImpressions(vector<Website> weblist,vector<Website> result_list)
{
    for(int i=0;i<result_list.size();i++)
    {
        weblist[result_list[i].node].ctr.impressions++;
    }
    return weblist;
}
void updateFile(vector<Website> weblist)
{
    ofstream impressionsfile;
    impressionsfile.open("impressions.csv");
    //file opening exception handling
    if(impressionsfile.is_open())
    for(int i=0;i<weblist.size();i++)
    {
        impressionsfile<<weblist[i].url<<","<<weblist[i].ctr.impressions<<endl;
    }
    cout<<"data writing successful\n";
}
void printList(vector<Website> result_list)
{
    cout<<"Results:\n";
    for(int i=0;i<result_list.size();i++)
    {
        cout<<i+1<<".  "<<result_list[i].url<<endl;
    }
    cout<<endl;
}