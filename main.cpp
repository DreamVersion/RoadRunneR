#include <iostream>
#include <vector>
#include <assert.h>

using namespace std;

const int SBOXSIZE=4;
const int BLOCKSIZE=64;
const int KEYSIZE=80;
const int ROUND=10;

int SBox[]={0,8,6,0xD,5,0xF,7,0xC,4,0xE,2,3,9,1,0xB,0xA};

int P[]={0,4,8,12,16,20,24,28,
         1,5,9,13,17,21,25,29,
         2,6,10,14,18,22,26,30,
         3,7,11,15,19,23,27,31};

int P_reverse[]={0,8,16,24,
                 1,9,17,25,
                 2,10,18,26,
                 3,11,19,27,
                 4,12,20,28,
                 5,13,21,29,
                 6,14,22,30,
                 7,15,23,31};

int BitsToInt(vector<int>& input,int len)
{
    assert(input.size()==len);
    int output=0;
    for(int i=0;i<len;i++)
    {
        output<<=1;
        output|=(input[i]&1);
    }
    return output;
}

vector<int> IntToBits(int input,int len)
{
    vector<int> output(len,0);
    for(int i=len-1;i>=0;i--)
    {
        output[i]=input&1;
        input>>=1;
    }
    return output;
}

void KeyGenerate(vector<int> key,vector<int>& wk0,vector<int>& wk1,vector<vector<vector<int>>>& RKs)
{
    assert(key.size()==KEYSIZE);
    assert(wk0.size()==32);
    assert(wk1.size()==32);
    assert(RKs.size()==ROUND);

    vector<vector<int>> vec(KEYSIZE/16, vector<int>(16,0));

    for(int i=0;i<KEYSIZE;i++)
    {
        vec[i/KEYSIZE][i%KEYSIZE] = key[i];
    }

    for(int i=0;i<16;i++)
    {
        //WK0=A||B WK1=C||D
        wk0[i]=vec[0][i];
        wk0[i+16]=vec[1][i];
        wk1[i]=vec[2][i];
        wk1[i+16]=vec[3][i];
        //RK0,RK5=(C||D)-(E||A)-(B||C)
        RKs[0][0][i]=vec[2][i];
        RKs[0][0][i+16]=vec[3][i];
        RKs[5][0][i]=vec[2][i];
        RKs[5][0][i+16]=vec[3][i];

        RKs[0][1][i]=vec[4][i];
        RKs[0][1][i+16]=vec[0][i];
        RKs[5][1][i]=vec[4][i];
        RKs[5][1][i+16]=vec[0][i];

        RKs[0][2][i]=vec[1][i];
        RKs[0][2][i+16]=vec[2][i];
        RKs[5][2][i]=vec[1][i];
        RKs[5][2][i+16]=vec[2][i];

        //RK1,RK6=(D||E)-(A||B)-(C||D)
        RKs[1][0][i]=vec[3][i];
        RKs[1][0][i+16]=vec[4][i];
        RKs[6][0][i]=vec[3][i];
        RKs[6][0][i+16]=vec[4][i];

        RKs[1][1][i]=vec[0][i];
        RKs[1][1][i+16]=vec[1][i];
        RKs[6][1][i]=vec[0][i];
        RKs[6][1][i+16]=vec[1][i];

        RKs[1][2][i]=vec[2][i];
        RKs[1][2][i+16]=vec[3][i];
        RKs[6][2][i]=vec[2][i];
        RKs[6][2][i+16]=vec[3][i];
        //RK2,RK7=(E||A)-(B||C)-(D||E)
        RKs[2][0][i]=vec[4][i];
        RKs[2][0][i+16]=vec[0][i];
        RKs[7][0][i]=vec[4][i];
        RKs[7][0][i+16]=vec[0][i];

        RKs[2][1][i]=vec[1][i];
        RKs[2][1][i+16]=vec[2][i];
        RKs[7][1][i]=vec[1][i];
        RKs[7][1][i+16]=vec[2][i];

        RKs[2][2][i]=vec[3][i];
        RKs[2][2][i+16]=vec[4][i];
        RKs[7][2][i]=vec[3][i];
        RKs[7][2][i+16]=vec[4][i];
        //RK3,RK8=(A||B)-(C||D)-(E||A)
        RKs[3][0][i]=vec[0][i];
        RKs[3][0][i+16]=vec[1][i];
        RKs[8][0][i]=vec[0][i];
        RKs[8][0][i+16]=vec[1][i];

        RKs[3][1][i]=vec[2][i];
        RKs[3][1][i+16]=vec[3][i];
        RKs[8][1][i]=vec[2][i];
        RKs[8][1][i+16]=vec[3][i];

        RKs[3][2][i]=vec[4][i];
        RKs[3][2][i+16]=vec[0][i];
        RKs[8][2][i]=vec[4][i];
        RKs[8][2][i+16]=vec[0][i];
        //RK4,RK9=(B||C)-(D||E)-(A||B)
        RKs[4][0][i]=vec[1][i];
        RKs[4][0][i+16]=vec[2][i];
        RKs[9][0][i]=vec[1][i];
        RKs[9][0][i+16]=vec[2][i];

        RKs[4][1][i]=vec[3][i];
        RKs[4][1][i+16]=vec[4][i];
        RKs[9][1][i]=vec[3][i];
        RKs[9][1][i+16]=vec[4][i];

        RKs[4][2][i]=vec[0][i];
        RKs[4][2][i+16]=vec[1][i];
        RKs[9][2][i]=vec[0][i];
        RKs[9][2][i+16]=vec[1][i];

    }
}

vector<int> SBoxFunction(vector<int>& input)
{
    assert(input.size()==SBOXSIZE);
    int src = BitsToInt(input,SBOXSIZE);
    int dst = SBox[src];
    vector<int> output = IntToBits(dst,SBOXSIZE);
    return output;
}

vector<int> PFunction(vector<int>& input)
{
    assert(input.size()==BLOCKSIZE/2);
    vector<int> output(BLOCKSIZE/2,0);

    for(int i=0;i<BLOCKSIZE/2;i++)
        output[P[i]]=input[i];

    return output;
}

vector<int> PReverseFunction(vector<int>& input)
{
    assert(input.size()==BLOCKSIZE/2);
    vector<int> output(BLOCKSIZE/2,0);

    for(int i=0;i<BLOCKSIZE/2;i++)
        output[P_reverse[i]]=input[i];

    return output;
}

vector<int> Layer(vector<int>& input)
{
    assert(input.size()==8);
    vector<int> output(8,0);
    for(int i=0;i<8;i++)
        output[i]=input[i]^input[(i+1)%8]^input[(i+2)%8];
    return output;
}

vector<int> Xor(vector<int>& xor_1,vector<int>& xor_2)
{
    assert(xor_1.size()==xor_2.size());
    int len = xor_1.size();
    vector<int> output(len);

    for(int i=0;i<len;i++)
        output[i]=xor_1[i]^xor_2[i];

    return output;
}

vector<int> SLK(vector<int>& input,vector<int>& RK)
{
    assert(input.size()==BLOCKSIZE/2);
    assert(RK.size()==BLOCKSIZE/2);
    vector<int> S_Ins = PFunction(input);
    vector<int> S_Outs(BLOCKSIZE/2,0);

    for(int i=0;i<8;i++)
    {
        vector<int> sin(4);
        for(int j=0;j<4;j++)
            sin[j]=S_Ins[4*i+j];
        vector<int> sout=SBoxFunction(sin);
        for(int j=0;j<4;j++)
            S_Outs[4*i+j]=sout[j];
    }

    vector<int> Layer_Ins = PReverseFunction(S_Outs);
    vector<int> Layer_Outs(BLOCKSIZE/2,0);

    for(int i=0;i<4;i++)
    {
        vector<int> layer_in(8);
        for(int j=0;j<8;j++)
            layer_in[j] = Layer_Ins[8*i+j];

        vector<int> layer_out = Layer(layer_in);
        for(int j=0;j<8;j++)
            Layer_Outs[8*i+j] = layer_out[j];
    }

    vector<int> res= Xor(Layer_Outs,RK);
    return res;
}

vector<int> F(vector<int>& input,vector<int>& C,vector<vector<int> >& RK)
{
    assert(RK.size()==3);

    vector<int> s1 = SLK(input,RK[0]);
    vector<int> s2 = SLK(s1,RK[1]);
    vector<int> s3 = Xor(C,s2);
    vector<int> s4 = SLK(s3,RK[2]);
    vector<int> s5 = PFunction(s4);
    vector<int> s6(BLOCKSIZE/2,0);

    for(int i=0;i<8;i++)
    {
        vector<int> sin(4);
        for(int j=0;j<4;j++)
            sin[j]=s5[4*i+j];
        vector<int> sout=SBoxFunction(sin);
        for(int j=0;j<4;j++)
            s6[4*i+j]=sout[j];
    }
    //验证是否有此步骤
    vector<int> s7 = PReverseFunction(s6);
    return s7;
}


vector<int> encrypt(const vector<int>& plaintext,const vector<int>& key)
{
    assert(plaintext.size()==BLOCKSIZE);
    assert(key.size()==KEYSIZE);

    vector<int> WK0(BLOCKSIZE/2);
    vector<int> WK1(BLOCKSIZE/2);

    vector<vector<vector<int>>> RKs(ROUND, vector< vector<int> >(3,vector<int>(32,0)));

    KeyGenerate(key,WK0,WK1,RKs);

    vector<int> Left(BLOCKSIZE/2,0);
    vector<int> Right(BLOCKSIZE/2,0);

    for(int i=0;i<BLOCKSIZE/2;i++)
    {
        Left[i]=plaintext[i];
        Right[i]=plaintext[i+32];
    }

    Left = Xor(Left,WK0);

    for(int i=0;i<ROUND;i++)
    {
        vector<int> C = IntToBits(ROUND-i,32);

        vector<int> tmp=F(Left,C,RKs[i]);
        vector<int> tmp2=Xor(Right,tmp);
        if(i<ROUND-1)
        {
            Right = Left;
            Left = tmp2;
        }
        else{
            Right=tmp2;
        }
    }

    Left=Xor(WK1,Left);

    vector<int> res(BLOCKSIZE);
    for(int i=0;i<BLOCKSIZE/2;i++)
    {
        res[i]=Left[i];
        res[i+32]=Right[i];
    }
    return res;
}
int main() {

//    int bits[]={1,0,1,1};
//    vector<int> bitsVec(bits,bits+sizeof(bits)/ sizeof(int));
//    cout<<"Test: Bits to int"<<endl;
//    cout<<BitsToInt(bitsVec,4)<<endl;
//
//    vector<int> res = IntToBits(11,8);
//    cout<<"Test: Int to bits"<<endl;
//    for(int i=0;i<res.size();i++)
//        cout<<res[i]<<" ";
//    cout<<endl;
//
//    cout<<"Test: Sbox function"<<endl;
//    for(int i=0;i<16;i++)
//    {
//        vector<int> sbox_input = IntToBits(i,SBOXSIZE);
//
//        cout<<"Input "<<i<<" :";
//        for(int i=0;i<sbox_input.size();i++)
//            cout<<sbox_input[i]<<" ";
//
//        vector<int> sbox_output = SBoxFunction(sbox_input);
//        cout<<endl<<"Output "<<BitsToInt(sbox_output,SBOXSIZE)<<" :";
//        for(int i=0;i<sbox_output.size();i++)
//            cout<<sbox_output[i]<<" ";
//        cout<<endl;
//    }
//
//    cout<<"Test: P function"<<endl;
//    vector<int> p_input;
//    for(int i=0;i<BLOCKSIZE/2;i++)
//        p_input.push_back(i);
//
//    vector<int> p_output = PFunction(p_input);
//    for(int i=0;i<BLOCKSIZE/2;i++)
//        cout<<p_output[i]<<" ";
//    cout<<endl;
//
//    vector<int> p_reverse_output = PReverseFunction(p_input);
//    for(int i=0;i<BLOCKSIZE/2;i++)
//        cout<<p_reverse_output[i]<<" ";
//    cout<<endl;
//
//    cout<<"Test: Layer"<<endl;
//    int layer_in[]={0,0,0,0,0,0,0,1};
//    vector<int> layer_input(layer_in,layer_in+ sizeof(layer_in)/ sizeof(int));
//    vector<int> layer_output = Layer(layer_input);
//    for(int i=0;i<layer_output.size();i++)
//        cout<<layer_output[i]<<" ";
//    cout<<endl;
//
//    cout<<"Test: SLK"<<endl;
//    vector<int> slk_in(BLOCKSIZE/2,0);
//    slk_in[BLOCKSIZE/2-1]=1;
//    vector<int> RK(BLOCKSIZE/2,0);
//
//    vector<int> slk_out = SLK(slk_in,RK);
//    for(int i=0;i<slk_out.size();i++)
//        cout<<slk_out[i]<<" ";
//    cout<<endl;

    vector<int> plaintext(BLOCKSIZE,0);
    vector<int> key(KEYSIZE,0);

//    for(int i=0;i<BLOCKSIZE;i++)
//        plaintext[i]=0;
//
//    for(int i=0;i<KEYSIZE;i++)
//        key[i]=0;

    vector<int> message = encrypt(plaintext,key);
    for(int i=0;i<message.size();i++)
        cout<<message[i]<<" ";
    cout<<endl;

    return 0;
}