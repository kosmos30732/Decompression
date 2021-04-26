#include <iostream>
#include <fstream>
#include <cstddef>
#include <list>
#include <vector>
#include <map>
#include <string>
#define NUM_OF_CHARS 256
using namespace std;

//our class that contains the symbol, the frequency of its occurrence, the left and right descendant
class Node
{
public:
    unsigned char token;
    int freq;
    Node* left, * rigth;

    Node()
    {
        token = 0;
        freq = 0;
        left = nullptr;
        rigth = nullptr;
    }

    Node(unsigned char _token, int _freq)
    {
        token = _token;
        freq = _freq;
        left = nullptr;
        rigth = nullptr;
    }

    Node(Node* _left, Node* _rigth)
    {
        token = 0;
        freq = _left->freq + _rigth->freq;
        left = _left;
        rigth = _rigth;
    }

    ~Node()
    {

    }
};


//the left - recursive function of traversing the tree and building to the leaves of their codes
void TreeGo(Node* head, vector<bool>code, map<char, vector<bool>>* list_code)
{
    if (head->left != nullptr)
    {
        vector <bool> temp = code;
        temp.push_back(0);
        TreeGo(head->left, temp, list_code);
    }

    if (head->rigth != nullptr)
    {
        vector <bool> temp = code;
        temp.push_back(1);
        TreeGo(head->rigth, temp, list_code);
    }

    //we got to the sheet and write down the code
    if (head->left == nullptr && head->rigth == nullptr)
    {
        (*list_code)[head->token] = code;
    }
}

int main()
{
    ifstream fin("output.txt", ios::binary);
    int n, symbol_freq = 0;

    //reading the first number, how many bits are occupied in the last byte
    unsigned char count = fin.get();

    //reading the number how many characters will we have is in the map
    {
        string t;
        unsigned char c = fin.get();
        while (true)
        {
            c = fin.get();
            if (c == ' ')
            {
                break;
            }
            t += c;
        }
        n = stoi(t);
    }

    //reading the characters themselves and the frequency of that character and writing them to a list
    unsigned char symbol;
    list<Node*>freq_list;
    for (size_t i = 0; i < n; i++)
    {
        symbol = fin.get();
        {
            string t;
            unsigned char c;
            while (true)
            {
                c = fin.get();
                if (c == ' ')
                {
                    break;
                }
                t += c;
            }
            symbol_freq = stoi(t);
        }

        Node* temp = new Node(symbol, symbol_freq);
        freq_list.push_back(temp);
    }

    //building a tree from our list
    while (freq_list.size() != 1)
    {
        Node* l = freq_list.front();
        freq_list.pop_front();
        Node* r = freq_list.front();
        freq_list.pop_front();

        Node* new_node = new Node(l,r);

        auto iter = freq_list.begin();

        for (; iter != freq_list.end();)
        {
            if (new_node->freq > (*iter)->freq)
            {
                iter++;
                continue;
            }
            else
            {
                break;
            }
        }
        freq_list.insert(iter, new_node);
    }

    //building codes for leaves and writing them to map
    vector<bool> code;
    map<char, vector<bool>>* list_code = new map<char, vector<bool>>;
    TreeGo(*(freq_list.begin()), code, list_code);

    //changing the key and value in map
    map<vector<bool>, char>* code_list = new map<vector<bool>, char>;
    for (auto it : *list_code)
    {
        (*code_list)[it.second] = it.first;
    }

    ofstream fout("output_output.txt");
    unsigned char c = 0;
    vector <bool> temp;

    //reading the next character from a compressed file and expanding them into text
    while (((c = fin.get())|| c==0) && !fin.eof())
    {
        //it is necessary to check the last byte, whether the file has ended
        auto end_of_file = fin.peek();
        for (size_t i = 0; i < 8; i++)
        {
            //adding the next bit of the read character to the temporary vector
            if (bool((1 << (7 - i)) & c))
            {
                temp.push_back(1);
            }
            else
            {
                temp.push_back(0);
            }

            //check if this sequence of bits is contained in map then output the text character
            auto iter = code_list->find(temp);
            if (iter == code_list->end())
            {
                continue;
            }
            temp.clear();
            fout << iter->second;

            //for the last byte, check that we have reached the end and written all the significant bits in the last byte
            if (end_of_file == EOF && i + 1 == count - '0')
            {
                break;
            }
        }
    }

    fout.close();
    fin.close();

    return 0;
}
