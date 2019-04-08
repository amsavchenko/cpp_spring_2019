#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_


enum class Error
{
    NoError,
    CorruptedArchive
};

class Serializer
{
public:
    explicit Serializer(std::ostream& out)
        : out_(out)
    {
    }
    
    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }
    
    template <class ... Args>
    Error operator() (Args&& ... args)
    {
        return process(std::forward<Args>(args) ...);
    }
    
private:
    static constexpr char Separator = ' ';
    std::ostream& out_;
    
    template <class T, class ... Args>
    Error process(T&& value, Args&& ... args)
    {
        if(process(std::forward<T>(value)) == Error::CorruptedArchive)
            return Error::CorruptedArchive;
        else
            return process(std::forward<Args>(args) ...);
    }
    
    Error process (bool& value)
    {
        out_ << (value ? "true" : "false") << Separator;
        return Error::NoError;
    }
    
    Error process (uint64_t& value)
    {
        out_ << value << Separator;
        return Error::NoError;
    }
    
    template <class T>
    Error process (T&& value)
    {
        return Error::CorruptedArchive;
    }
    
};


class Deserializer
{
public:
    explicit Deserializer(std::istream& in)
        : in_(in)
    {
    }
    
    template <class T>
    Error load(T& object)
    {
        return object.serialize(*this);
    }
    
    template <class ... Args>
    Error operator() (Args&& ... args)
    {
        return process(std::forward<Args>(args) ...);
    }
    
    
private:
    static constexpr char Separator = ' ';
    std::istream& in_;
    
    template <class T, class ... Args>
    Error process (T&& value, Args&& ... args)
    {
        if (process(std::forward<T>(value)) == Error::CorruptedArchive)
            return Error::CorruptedArchive;
        else
            return process(std::forward<Args>(args) ...);
    }
    
    Error process (bool& value)
    {
        std::string str;
        in_ >> str;
        
        if (str == "true")
            value = true;
        else if (str == "false")
            value = false;
        else
            return Error::CorruptedArchive;
        
        return Error::NoError;
    }
    
    Error process (uint64_t& value)
    {
        std::string str;
        in_ >> str;
        
        if (str.empty() || str[0] == '-' || str[0] == '\0')
            return Error::CorruptedArchive;
        value = static_cast<uint64_t>(std::stoi(str));
        return Error::NoError;
    }
    
};



#endif
