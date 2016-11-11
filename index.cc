#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include <string.h>       // provides string class
#include <cctype>         // provides isalpha() and tolower()
#include <ctime>                // for time()
#include <cstdlib>              // for rand(), srand(), and RAND_MAX
#include <time.h>

using namespace std;

ofstream fout;
int pageNum=1,lineNum=1,size=0; // size is the number of words

//Function Signatures
int randBit();

//Dictionary class
class Dictionary{

		 vector<string> words; //Vector of all the words
		 vector<string> copy;	//copy for sorting later
		 vector<int> numTimes;	//numbers of times word of index i in words appears is stored in index i of this vector
		 vector<vector<int> > chapters;  //Stores which chapter each word appears in in a vector at index i
	
	//Returns the index of string word if it is contained in the dictionary
	public: 
		//Constructor
		Dictionary (vector<string>,vector<int>,vector<int>);

		//Returns the index of word of -1 if it doesn't appear in the list
		int linearSearch(string  word){
        		for(long i=0;i<words.size();i++){
                	 	if(word==words[i]){
                         		return i;
                 		}
         		}
         		return -1;
	 	}
		
		//Inserts new word into the end of the dictionary or modifies size and page numbers appropriatly
		int insert(string word){
			int i=linearSearch(word);
			int found=0;

			if(i==-1){
				words.push_back(word);
				copy.push_back(word);
				numTimes.push_back(1);

				vector<int> row;
				row.push_back(pageNum);
				chapters.push_back(row);
				
			}else{
				numTimes[i]=numTimes[i]+1;
				
				for(int h=0;h<chapters[i].size();h++){
					if(chapters[i][h]==pageNum)
						found=1;
				}
				if(found!=1)
					chapters[i].push_back(pageNum);	
			}
		}

		void printInOrder(){
			sort(copy.begin(),copy.end());
			int j;

			for(int i=0;i<words.size();i++){
				j=linearSearch(copy[i]);
				std::cout<<copy[i]<<"  ("<< numTimes[j]<<")  ";
				fout<<copy[i]<<"  ("<< numTimes[j]<<")  ";
				for(int k=0;k<chapters[j].size()-1;k++){
					cout<<chapters[j][k]<<",";
					fout<<chapters[j][k]<<",";
				}
					cout<<chapters[j][chapters[j].size()-1]<<"\n";
					fout<<chapters[j][chapters[j].size()-1]<<"\n";
			}
		
		}
};

Dictionary::Dictionary(vector<string> words, vector<int> numTimes, vector<int> chapters){
		words=words;
		numTimes=numTimes;
		chapters=chapters;
}

int randBit(void) {             // return a "random" bit
  static int bitsUpperBd=0;
  static int bits;              // store bits returned by rand()
  if( bitsUpperBd == 0 ) {      // refresh store when empty
    bitsUpperBd = RAND_MAX;
    bits = rand();
  }
  int b = bits & 1;
  bits >>= 1;
  bitsUpperBd >>= 1;
  return b;
}

int randomHeight(){
	int height;
	//Max height is 16
	for(int i=1;i<16;++i){
		if(randBit()==1){
			height=i;
			break;
		}
	}
	return height;
}
// Remove all characters except letters (A-Z,a-z) from line,
// except keep '-' or '\'' if they are between letters.
void lowercaseWords(string & line) {
  for( string::iterator it = line.begin(); it != line.end(); ++it ) {
    if( !isalpha(*it) ) {
      if( (*it != '-' && *it != '\'') ||
          it == line.begin() || it+1 == line.end() ||
          !isalpha(*(it-1)) || !isalpha(*(it+1)) ) {
        *it = ' ';
      }
    } else {
      *it = tolower(*it);
    }
  }
}

void print(ostream & output, string & word) {
  output << word << endl;
}


//Struct to be used in the SkipNode struct
struct Data{
	string word;
	int occurences;
	int pageNum_vec[10]; // Simply a number bigger than 6, the number of pages in the chapter 

	public:
		Data(string text);
};

Data::Data(string text){
	word=text;
	occurences=1;
	for(int i=0;i<8;i++) // only important to set indices 1-6 since there are only 6 pages
		pageNum_vec[i]=0;
	//Initialize array to 0's but then if the word appears on page 4 set array[4] to 4;
	pageNum_vec[pageNum]=pageNum;
}

class SkipList{

	struct SkipNode{
        	Data* data;
	        int height;
        	//16 is max height
	        SkipNode* next[16];

	};

	SkipNode* head;

	public:
		SkipList();
		
		//Search as per the instruction given on handout
		Data* search(string text){
			SkipNode* node=head;
			//This is max height because node has head's height and head is max height
			int h=(node->height)-1;
                        for(h;h>=0;h--){
                                while(node->next[h]!=NULL && node->next[h]->data->word.compare(text)<0){
                                        node=node->next[h];
                                }
                                if(node->next[h]!=NULL && node->next[h]->data->word.compare(text)==0){
                                     	return node->next[h]->data;
                                }
                        }

			return NULL;
		}
	
		//Insert as per instructions given on handout
		//returns 1 if found, 0 otherwise
		int insert(string text){
			//First see if word exists, redo search with added lines
                        SkipNode* node=head;
			SkipNode* update[16]; // 16 is max height
                        //This is max height because node has head's height and head is max height
                        int h=(node->height)-1;
			int found=0;

                        for(h;h>=0;h--){
				while(node->next[h]!=NULL && node->next[h]->data->word.compare(text)<0){
					node=node->next[h];
				}
				if(node->next[h]!=NULL && node->next[h]->data->word.compare(text)==0){
                                                //If you found it edit the data of the SkipNode
                                               node->next[h]->data->occurences=node->next[h]->data->occurences+1;

                                               node->next[h]->data->pageNum_vec[pageNum]=pageNum;
                                                
                                                update[h]=node;
						found=1;
						return 1;					
				}
				update[h]=node;
			}
			if(found==0){
				//If you don't find it create a new SkipNode
				SkipNode* x = new SkipNode;
				x->data= new Data(text);
				x->height= randomHeight();
			
				//Change the pointers as per instructions in the handout
				for(int h=0;h<x->height;++h){
					x->next[h]=update[h]->next[h];
					update[h]->next[h]=x;
				}
			}
			return 0;
		}
		void printInOrder(){
			SkipNode* n = head->next[0];
			vector<int> indices;
			int line=0; // to keep track of whether I have already printed the -
			while(n){
				fout<<n->data->word<<" ("<<n->data->occurences<<") ";
				//i<7 because only 6 pages, starts at page 1
				for(int i=1;i<7;i++){
					if(n->data->pageNum_vec[i]!=0)
						indices.push_back(n->data->pageNum_vec[i]);
				}

				fout<<indices[0];
				//Logic for determing when to print a comma or a line
				for(int k=1;k<indices.size()-1;k++){
					if(indices[k+1]-indices[k]!=1){
						fout<<","<<indices[k];
					}else{
						if(line!=1){
							fout<<"-";
						}
						line=1;
					}
				}
				//Print the last element of indices not covered by loop
				if(indices.size()>1){
					if(indices[indices.size()-1]-indices[indices.size()-2]==1 && line !=1)
						fout<<"-"<<indices[indices.size()-1];
					else if(indices[indices.size()-1]-indices[indices.size()-2]!=1)
						fout<<","<<indices[indices.size()-1];
					else
						fout<<indices[indices.size()-1];
				}
/*				for(int k=0;k<indices.size()-1;k++){
					cout<<indices[k]<<",";
					fout<<indices[k]<<",";
				}
				cout<<indices[indices.size()-1];
				fout<<indices[indices.size()-1];*/
				fout<<"\n";
				line=0;
				n=n->next[0];
				indices.clear();
			}
		}
};

SkipList::SkipList(){
	head= new SkipNode;
	head->data=NULL;

	//Max height is 16
	head->height=16;
}

int main(int argc, char *argv[]) {
	//Part 1 Variables
	vector <string> words,copy;
	vector <int> numTimes;
	vector <int> chapters;
	Dictionary d(words,numTimes,chapters);

	//Part 2 Variables
	srand(time(0));	
	SkipList s;
	

  if( argc != 3 ) {
    cerr << "Usage: " << argv[0] << " book.txt index.txt" << endl;
    exit(1);
  }

  ifstream fin;
  fin.open(argv[1]);
  if( !fin.is_open() ) {
    cerr << "Unable to open " << argv[1] << endl;
    exit(2);
  }

  //ofstream fout;
  fout.open(argv[2]);
  if( !fout.is_open() ) {
    cerr << "Unable to open " << argv[2] << endl;
    exit(3);
  }

  string line, word;

  while( !fin.eof() ) {
    getline(fin,line);
    lowercaseWords(line);
    istringstream iss(line, istringstream::in);
    while( iss >> word ) {
      	//Add words to vector array
	pageNum=1+lineNum/40;
	//cout<<word<<"\n";
	//d.insert(word);
	//size++;
	s.insert(word);
    }
	//This is to keep track of the page which the words are on
	lineNum++;
  }
	s.printInOrder();
}




