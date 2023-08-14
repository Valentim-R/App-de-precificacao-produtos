#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <windows.h>

#define IMPOSTO "986547125"
#define VALOR_MIN_FRETE "009069063"
#define VALOR 1
#define TAXA_CLASSICO 1
#define TAXA_PREMIUM 2
#define TAXA_FIXA_SFRETE_GRATIS "099863785"
#define MARGEM_ERRO 0.000001

using namespace std;

int Comparacao_Strings(string str_1, string str_2)
{
    int i = 0, Tam_1 = str_1.length(), Tam_2 = str_2.length();

    if (str_1.compare(str_2) > 0)
    {
        for (int j = 0; j < Tam_1; j++)
        {
            if (str_1[j] != str_2[j])
                i++;
        }
    }
    else
    {
        for (int j = 0; j < Tam_2; j++)
        {
            if (str_1[j] != str_2[j])
                i++;
        }
    }
    return i;
}

float Ler_Documento_Taxas(string _Categoria, int _TipoAnuncio)
{
    ifstream InFile;
    string Auxiliar;
    bool i = true;
    InFile.open("config.txt", ios::in);
    if (!InFile)
    {
        cout << "Nao foi possivel abrir o arquivo\nCriando um novo e reiniciando programa";
        ofstream OutFile;
        OutFile.open("config.txt");
        OutFile.close();
        abort();
    }
    while (i == true && !InFile.eof())
    {
        InFile >> Auxiliar;
        if (Comparacao_Strings(_Categoria, Auxiliar) <= (int)Auxiliar.length() / 4)
        {
            for (int f = 0; f < _TipoAnuncio; _TipoAnuncio--)
            {
                InFile >> Auxiliar;
                i = false;
            }
        }
    }
    if (InFile.eof())
        cout << "Nao foi possivel achar oque esta proucurando";
    InFile.close();

    return stof(Auxiliar);
}

float Tabelacao_Frete(float _Peso_Gramas, int _Valor)
{
    ifstream InFile;
    float Auxiliar;
    bool i = true;
    InFile.open("Tabela_Frete.txt", ios::in);
    if (!InFile)
    {
        cout << "Nao foi possivel abrir o arquivo\nCriando um novo e reiniciando programa";
        ofstream OutFile;
        OutFile.open("Tabela_Frete.txt");
        OutFile.close();
        abort();
    }
    while (i == true && !InFile.eof())
    {
        InFile >> Auxiliar;
        if (Auxiliar >= _Peso_Gramas)
        {
            for (int f = 0; f < _Valor; _Valor--)
            {
                InFile >> Auxiliar;
                i = false;
            }
        }
    }
    if (InFile.eof())
        cout << "Nao foi possivel achar oque esta proucurando";
    InFile.close();
    return Auxiliar;
}

void Acressimo_Porcentagens(float *_Valor_Venda, float *Porcentagem)
{
    *_Valor_Venda = *_Valor_Venda + (*_Valor_Venda * *Porcentagem);
}

void Remocao_Imposto_Taxa(float *_A_Receber, float *_Imposto, float *_Taxa)
{
    *_A_Receber = *_A_Receber - (*_A_Receber * (*_Imposto + *_Taxa));
}

void Verificacao_Tomada_Decisao(float *_A_Receber, float *_Imposto, float *_Taxa, float *_Valor_Custo, float *_Frete, float *_Lucro)
{
    bool i = false;

    Remocao_Imposto_Taxa(_A_Receber, _Imposto, _Taxa);

    if (*_A_Receber <= Ler_Documento_Taxas(VALOR_MIN_FRETE, VALOR))
        i = true;
    else
        i = false;
    if (i)
        *_A_Receber -= Ler_Documento_Taxas(TAXA_FIXA_SFRETE_GRATIS, VALOR);
    else
        *_A_Receber -= *_Frete;

    *_Lucro = (*_A_Receber - *_Valor_Custo) / *_Valor_Custo;
}

float Precificacao_MarketPlace(float _Valor_Custo, string _Categoria, float _Peso_Gramas, float _Margem_Lucro, float *Lucro, float *Taxa)
{
    float Frete = Tabelacao_Frete(_Peso_Gramas, VALOR);
    float Taxa_Classico = Ler_Documento_Taxas(_Categoria, TAXA_CLASSICO);
    float Taxa_Premium = Ler_Documento_Taxas(_Categoria, TAXA_PREMIUM);
    float Imposto = Ler_Documento_Taxas(IMPOSTO, VALOR);
    float _Valor_Venda_Premium = _Valor_Custo + Frete;
    float _Valor_Venda_SFrete = _Valor_Custo + Ler_Documento_Taxas(TAXA_FIXA_SFRETE_GRATIS, VALOR);
    float A_Receber = 0;
    float Valor_Venda = 0;

    // Acressimo Margem de Imposto --------------------------------------------------------
    Acressimo_Porcentagens(&_Valor_Venda_Premium, &Imposto);
    Acressimo_Porcentagens(&_Valor_Venda_SFrete, &Imposto);

    // Acressimo Margem de Taxa -----------------------------------------------------------
    Acressimo_Porcentagens(&_Valor_Venda_Premium, &Taxa_Premium);
    Acressimo_Porcentagens(&_Valor_Venda_SFrete, &Taxa_Classico);

    // Acressimo Margem de Lucro ----------------------------------------------------------
    Acressimo_Porcentagens(&_Valor_Venda_Premium, &_Margem_Lucro);
    Acressimo_Porcentagens(&_Valor_Venda_SFrete, &_Margem_Lucro);

    // Verificação para tomada de decisão -------------------------------------------------
    if (_Valor_Venda_SFrete <= 78.90)
    {
        Valor_Venda = _Valor_Venda_SFrete;
        *Taxa = Taxa_Classico;
        A_Receber = _Valor_Venda_SFrete;
        Verificacao_Tomada_Decisao(&A_Receber, &Imposto, &Taxa_Classico, &_Valor_Custo, &Frete, Lucro);
    }
    else
    {
        Valor_Venda = _Valor_Venda_Premium;
        *Taxa = Taxa_Premium;
        A_Receber = _Valor_Venda_Premium;
        Verificacao_Tomada_Decisao(&A_Receber, &Imposto, &Taxa_Premium, &_Valor_Custo, &Frete, Lucro);
    }

    // Acionamento para ajuste do lucro -------------------------------------
    float Necessidade = 0;
    int i = 0;
    do
    {
        i = 0;
        while ((*Lucro < _Margem_Lucro || *Lucro > _Margem_Lucro + MARGEM_ERRO) && i < 1000)
        {
            Necessidade = _Margem_Lucro - *Lucro;
            Acressimo_Porcentagens(&Valor_Venda, &Necessidade);
            A_Receber = Valor_Venda;
            Remocao_Imposto_Taxa(&A_Receber, &Imposto, Taxa);

            if (*Taxa == Taxa_Classico)
                A_Receber -= Ler_Documento_Taxas(TAXA_FIXA_SFRETE_GRATIS, VALOR);
            else
                A_Receber -= Frete;

            if (*Taxa == Taxa_Classico && Valor_Venda > 78.90)
            {
                Valor_Venda = _Valor_Venda_Premium;
                *Taxa = Taxa_Premium;
            }
            *Lucro = (A_Receber - _Valor_Custo) / _Valor_Custo;
            i++;
        }
        if (i >= 1000)
        {
            if (_Valor_Venda_SFrete <= 78.90)
            {
                Valor_Venda = _Valor_Venda_SFrete;
                *Taxa = Taxa_Classico;
                A_Receber = _Valor_Venda_SFrete;
                Verificacao_Tomada_Decisao(&A_Receber, &Imposto, &Taxa_Classico, &_Valor_Custo, &Frete, Lucro);
            }
            else
            {
                Valor_Venda = _Valor_Venda_Premium;
                *Taxa = Taxa_Premium;
                A_Receber = _Valor_Venda_Premium;
                Verificacao_Tomada_Decisao(&A_Receber, &Imposto, &Taxa_Premium, &_Valor_Custo, &Frete, Lucro);
            }
            _Valor_Custo = _Valor_Custo + 1.0;
        }
    } while (i >= 1000);
    return Valor_Venda;
}

bool Verificar_Categoria(string _Categoria)
{
    ifstream InFile;
    string Auxiliar;
    bool i = false;
    InFile.open("config.txt", ios::in);
    if (!InFile)
    {
        cout << "Nao foi possivel abrir o arquivo\nCriando um novo e reiniciando programa";
        ofstream OutFile;
        OutFile.open("config.txt");
        OutFile.close();
        abort();
    }
    while (i == false && !InFile.eof())
    {
        InFile >> Auxiliar;
        if (Comparacao_Strings(_Categoria, Auxiliar) <= (int)Auxiliar.length() / 4)
        {
            cout << endl
                 << "Utilizando categoria " << Auxiliar << endl;
            i = true;
        }
    }
    if (InFile.eof())
    {
        cout << endl
             << "Nao foi possivel achar oque esta proucurando" << endl;
        ofstream OutFile;
        float x, y;
        OutFile.open("config.txt", ios::app);
        cout << "Adicionando Categoria, Digite o nome com '_' no lugar dos espacos: " << endl;
        cin >> _Categoria;
        cout << "Taxa Classico: " << endl;
        cin >> x;
        cout << "Taxa Premium: " << endl;
        cin >> y;
        x /= 100;
        y /= 100;
        OutFile << _Categoria << " " << x << " " << y << "\n";
        OutFile.close();
        i = true;
    }
    InFile.close();
    return i;
}

int main()
{
    float Peso_Gramas;
    float Valor_Custo;
    float Valor_Venda;
    float Margem_Lucro;
    float Lucro;
    float Taxa;
    string Categoria;

    do
    {
        do
        {
            cout << "Categoria: ";
            getline(cin, Categoria);
        } while (!Verificar_Categoria(Categoria));
        cout << endl;
        cout << "Peso do produto em gramas: ";
        cin >> Peso_Gramas;
        cout << "Valor de custo do produto: ";
        cin >> Valor_Custo;
        cout << "Margem de lucro desejada: ";
        cin >> Margem_Lucro;
        Margem_Lucro /= 100;

        Valor_Venda = Precificacao_MarketPlace(Valor_Custo, Categoria, Peso_Gramas, Margem_Lucro, &Lucro, &Taxa);

        cout << endl
             << "Valor de venda: R$ " << round(Valor_Venda * 100) / 100 << endl
             << "Lucro: " << round(Lucro * 10000) / 100 << "%" << endl;
        if (Taxa <= 0.14)
            cout << "Anuncio Classico" << endl
                 << endl;
        else
            cout << "Anuncio Premium" << endl
                 << endl;

        // Limpar buffer
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        getchar();
        system("cls");
    } while (true);
}