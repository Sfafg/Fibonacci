#pragma once 

// Klasa implementujaca w sposob softwarowy liczby calkowite nieujemne,
// TByteCount - na ilu bitach zapisywana jest liczba.
template<unsigned int TByteCount>
class BigInt
{
    unsigned char data[TByteCount]{};

public:
    BigInt(int n = 0)
    {
        for(int i = 0; i < TByteCount; i++)
        {
            data[i] = n & 255;
            n >>= 8;
            if(n == 0) break;
        }
    }

    BigInt(unsigned long long n )
    {
        for(int i = 0; i < TByteCount; i++)
        {
            data[i] = n & 255;
            n >>= 8;
            if(n == 0) break;
        }
    }
    
    template<unsigned int T>
    friend BigInt<T> operator+(const BigInt<T>& lhs, const BigInt<T>& rhs);
    template<unsigned int T>
    friend std::ostream& operator<<(std::ostream& os, const BigInt<T>& rhs);
};

template<unsigned int TByteCount>
BigInt<TByteCount> operator+(const BigInt<TByteCount>& lhs, const BigInt<TByteCount>& rhs)
{
    unsigned short carry = 0;
    unsigned short a = 0;
    unsigned short b = 0;
    BigInt<TByteCount> t;

    for(int i =0; i < TByteCount; i++)
    {
        a = lhs.data[i];
        b = rhs.data[i];
        unsigned short sum = a + b + carry;
        t.data[i] = sum & 255;
        carry = (sum >> 8) & 255;
    }

    if(carry != 0)
    {
        std::cerr << "Addition overflow\n";
        exit(1);
    }

    return t;
}

// Funkcja dodajaca liczby w reprezentacji decymalnej, gdzie kazdy element wektora to kolejna cyfra liczby,
void AddDecimalToA(std::vector<char>& a, const std::vector<char>& b)
{
    char carry = 0;
    int i = 0;
    if(a.size() < b.size())
        a.reserve(b.size()+1);

    for(; i < b.size(); i++)
    {
        if(a.size() <= i)
            a.push_back(0);

        int sum = a[i] + b[i] + carry;
        a[i] = sum % 10;
        carry = sum / 10;
    }

    while(carry != 0)
    {
        if(a.size() <= i)
            a.push_back(0);

        int sum = a[i] + carry;
        a[i] = sum % 10;
        carry = sum / 10;
        i++;
    }
}

// Funkcja mnozaca liczby w reprezentacji decymalnej, gdzie kazdy element wektora to kolejna cyfra liczby,
std::vector<char> MultDecimal(const std::vector<char>& a_, const std::vector<char>& b_)
{
    const std::vector<char>* a = &a_;
    const std::vector<char>* b = &b_;
    if(b_.size() < a_.size())
    {
        a = &b_;
        b = &a_;
    }

    std::vector<char> ret;
    for(int i = 0; i < a->size();i++)
    {
        for(int j = 0; j < b->size(); j++)
        {
            int product = (*a)[i] * (*b)[j];
            std::vector<char> decimalProduct;
            decimalProduct.reserve(j + i + 2);
            for(int k = 0; k < j + i; k++)
                decimalProduct.push_back(0);

            int productDigits = product % 10;
            int productTens = product / 10;
            if(productTens != 0)
            {
                decimalProduct.push_back(productDigits);
                decimalProduct.push_back(productTens);
            }
            else if(productDigits != 0)
                decimalProduct.push_back(productDigits);
            else continue;
            
            AddDecimalToA(ret, decimalProduct);
        }
    }

    return ret;
}

template<unsigned int TByteCount>
std::ostream& operator<<(std::ostream& os, const BigInt<TByteCount>& rhs)
{
    // Bardzo nieoptymalny kod, ale dziala i nie jest duzym problemem w czasie wykonywania.
    // Zamienia liczbe BigInt na reprezentacja decymalna a nastepnie ja wypisuje.
    std::vector<char> decimalNumber;
    std::vector<char> base = {6,5,2};
    for(int i = TByteCount - 1; i >= 0; i--)
    {
        if(decimalNumber.size() != 0)
            decimalNumber = MultDecimal(decimalNumber, base);

        int number = rhs.data[i];
        if(number == 0)
            continue;

        int digits = number % 10;
        int tens = (number / 10) % 10;
        int hundreds = number / 100;

        std::vector<char> temp;
        if(hundreds != 0)
        {
            temp.push_back(digits);
            temp.push_back(tens);
            temp.push_back(hundreds);
        }
        else if(tens != 0)
        {
            temp.push_back(digits);
            temp.push_back(tens);
        }
        else if(digits != 0)
            temp.push_back(digits);

        AddDecimalToA(decimalNumber, temp);
    }
    
    for(int i = decimalNumber.size()-1; i >= 0; i--)
        os << (int)decimalNumber[i];

    if(decimalNumber.size() == 0)
        os << 0;

    return os;
}
