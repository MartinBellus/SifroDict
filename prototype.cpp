#include<vector>
#include<string>
#include<iostream>
#include<algorithm>
#include<map>
#include<fstream>

using namespace std;

struct Node{
    map<char,Node*> children;
    Node* parent;
    char c;
    int freq = 0;
    Node(char ch,Node* p = nullptr,int f = 0){
        parent = p;
        c = ch;
        freq = f;
    }
    Node* add_child(char ch){
        if(children.find(ch) == children.end()) children[ch] = new Node(ch,this);
        return children[ch];
    }
};

struct Trie{
    Node* root;
    Trie(){
        root = new Node(':');
    }
    void match_pattern(string pattern,vector<pair<string,int>>& out,int ind = 0,Node* cur = nullptr){
        if(cur == nullptr) cur = root;
        if(cur->c == '$' && ind == pattern.size()){ //match
            int f = cur->freq;
            string ans = "";
            cur = cur->parent;
            while(cur->parent != nullptr){
                ans += cur->c;
                cur = cur->parent;
            }
            reverse(ans.begin(),ans.end());
            out.push_back({ans,f});
            return;
        }
        if(ind == pattern.size()) return; //no match
        if(pattern[ind] == '.'){ //resolve wildcard character
            for(auto it = cur->children.begin();it != cur->children.end();it++){
                match_pattern(pattern,out,ind + 1, it->second);
            }
        }
        if(cur->children.find(pattern[ind]) != cur->children.end()){
            match_pattern(pattern,out,ind + 1,cur->children[pattern[ind]]);    
        }
    }
    void deserialize(string dump){
        ifstream in;
        in.open(dump);
        while(!in.is_open()){
            cout<<"Could not open wordlist. Enter path to trie.dump file: ";
            string path;
            if(path == "q") return;
            cin>>path;
            in.open(path);
        }
        Node* nv = root;
        char c;in>>c;
        int freq;
        while(nv != nullptr){
            in>>c;
            if(c == '#') nv = nv->parent;
            else{
                nv = nv->add_child(c);
                if(c == '$'){in>>freq;nv->freq = freq;}
            }
        }
        cout<<"Wordlist loaded succesfully!"<<endl;
        in.close();
    }
};

Trie tr = Trie();

bool comp(pair<string,int> a,pair<string,int> b){
    return a.second > b.second || (a.second == b.second && a.first > b.first);
}

void manual_inp(){
    char id;
    string co;
    while(1){
        cin>>id;
        if(id == '?'){
            cin>>co;
            vector<pair<string,int>> ans;
            co += '$';
            tr.match_pattern(co,ans);
            sort(ans.begin(),ans.end(),comp);
            int ind = 0;
            while(ind < ans.size()){
                for(int cnt = 0;cnt <= 31 && ind < ans.size();cnt++,ind++){
                    cout<<ans[ind].first<<endl;
                }
                if(ind == ans.size()) break;
                cout<<"Show more results? [y/n] ";
                string a;cin>>a;
                if(a != "y" && a != "Y") break;
            }
        }else break;
    };
}

int main(){
    tr.deserialize("trie.dump");
    manual_inp();
}
