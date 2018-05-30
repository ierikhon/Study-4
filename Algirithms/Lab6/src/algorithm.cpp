#include "algorithm.h"

extern vector<Bor> bor;
extern vector<string> patterns;

Bor makeBor(int parentN, char sym)
{
    Bor borS;
    borS.next[sym] = borS.auto_move[sym] = -1;
    borS.isStr = false;
    borS.suffixL = -1;
    borS.parent = parentN;
    borS.symbol = sym;
    borS.CompressedSuffixL = -1;
    return borS;
}

void addtoBor(const string &s)
{
    int number = 0;
    for (size_t i=0; i<s.length(); ++i)
    {
        char sym = s[i];
        if ((bor[number].next.find(sym) == bor[number].next.end()) || (bor[number].next[sym] == -1))
        {
            bor.push_back(makeBor(number, sym));
            bor[number].next[sym] = bor.size() - 1;
        }
        number = bor[number].next[sym];
    }
    bor[number].isStr = true;
    patterns.push_back(s);
    bor[number].patternNumber.push_back(patterns.size() - 1);
}

int getLink(int node)
{
    if (bor[node].suffixL == -1)
    {
        if (node == 0 || bor[node].parent == 0)
            bor[node].suffixL = 0;
        else
            bor[node].suffixL = getAM(getLink(bor[node].parent), bor[node].symbol);
    }
    return bor[node].suffixL;
}

int getAM(int node, char sym)
{
    if ((bor[node].auto_move.find(sym) == bor[node].auto_move.end())||(bor[node].auto_move[sym] == -1))
    {
        if ((bor[node].next.find(sym) != bor[node].next.end())&&(bor[node].next[sym] != -1))
            bor[node].auto_move[sym] = bor[node].next[sym];
        else
        {
            if (node == 0)
                bor[node].auto_move[sym] = 0;
            else
                bor[node].auto_move[sym] = getAM(getLink(node), sym);
        }
    }
    return bor[node].auto_move[sym];
}

int getCompressedLink(int node)
{
    if (bor[node].CompressedSuffixL == -1)
    {
        int link = getLink(node);
        if (link == 0)
            bor[node].CompressedSuffixL = 0;
        else
        {
            if (bor[link].isStr)
                bor[node].CompressedSuffixL = link;
            else bor[node].CompressedSuffixL = getCompressedLink(link);
        }
    }
    return bor[node].CompressedSuffixL;
}

void AC(const string &s, map<int, vector<int>> &answer)
{
    int k = 0;
    for(size_t i = 0; i < s.length(); i++)
    {
        k = getAM(k, s[i]);
        for(int j = k; j != 0; j = getCompressedLink(j))
        {
            if (bor[j].isStr)
            {
                cout << i+1 - patterns[bor[j].patternNumber[0]].length() + 1 << " " << bor[j].patternNumber[0] + 1 << endl;
                answer[i+1 - patterns[bor[j].patternNumber[0]].size()+1].push_back(bor[j].patternNumber[0] + 1);
            }
        }
    }
}

void AC_UPGR(const string& s,vector<size_t>& count, const vector<int>& lenght)
{
    int u = 0;
    for(size_t i = 0; i < s.length(); i++)
    {
        u = getAM(u, s[i]);
        for(int j = u; j != 0; j = getCompressedLink(j))
            if (bor[j].isStr)
                for(const auto& current: bor[j].patternNumber)
                    if((size_t)(i+1-lenght[current])<count.size())
                        count[i+1-lenght[current]]++;
    }
}

vector<int> KMP_UPGR (const string &P, const string &pattern, char joker)
{
    vector<int> result;
    bor.push_back(makeBor(0, '$'));

    stringstream str_pat(pattern);
    vector<size_t> count(P.size(),0);

    vector<int> lenght;
    int len=0;
    string tmp;
    while(getline(str_pat, tmp, joker))
    {
        if(tmp.size()>0)
        {
            len+=tmp.size();
            lenght.push_back(len);
            addtoBor(tmp);
        }
        len++;
    }

    AC_UPGR(P, count, lenght);

    for(size_t i=0; i<P.size(); i++)
        if(count[i] == patterns.size())
            result.push_back(i+1);

    return result;
}

map<int,vector<int>> KMP (const string &P, const string &T)
{
    bor.push_back(makeBor(0, '$'));
    map<int,vector<int>> answer;
    patterns.push_back(P);
    AC(T, answer);
    return answer;
}