use std::fmt::Display;

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
pub enum PrimitiveType {
    Int,
    Float,
    Bool,
    Char,
    Void,
}

impl Display for PrimitiveType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            PrimitiveType::Int => write!(f, "int"),
            PrimitiveType::Float => write!(f, "float"),
            PrimitiveType::Bool => write!(f, "bool"),
            PrimitiveType::Char => write!(f, "char"),
            PrimitiveType::Void => write!(f, "void"),
        }
    }
}

#[derive(Clone, PartialEq, Debug)]
pub enum Token {
    Illegal,
    Eof,
    // 字面量
    IntLit(i32),
    FloatLit(f32),
    StrLit(String),
    CharLit(u8), // ascii char only
    // 关键字&标识符&基本类型
    BoolLit(bool),
    Null,
    If,
    Else,
    While,
    Break,
    Continue,
    Return,
    Extern,                       //声明外部函数
    Const,                        //声明编译时常量
    PrimitiveType(PrimitiveType), //基本类型
    Ident(String),
    // 算符
    Equal, // 声明, 赋值
    // 算术运算符
    Plus,    //加法
    Minus,   //减法/负号
    Star,    //乘法, 解引用, 声明指针类型
    Slash,   //除法
    Percent, //取余
    // 逻辑运算符
    AmpersandAmpersand, //逻辑与
    PipePipe,           //逻辑或
    Bang,               //逻辑非
    //关系运算符
    EqualEqual,   //判等
    BangEqual,    //判不等
    Less,         //小于
    Greater,      //大于
    LessEqual,    //小于等于
    GreaterEqual, //大于等于
    // 位运算符
    Tilde,     //按位取反
    Caret,     //按位异或
    Ampersand, //按位与, 取引用
    Pipe,      //按位或
    // 标点符号
    Comma,     //, 分割数组元素/函数参数/对象属性
    SemiColon, //; 语句末尾结束符, for分割
    LParen,    //( 函数调用/函数参数参数/表达式分组
    RParen,    //) 函数调用/函数参数/表达式分组
    LBrack,    //[ 数组索引/数组声明
    RBrack,    //] 数组索引/数组声明
    LBrace,    //{ 代码块/函数体/条件分支体/循环体/数组构造
    RBrace,    //} 代码块/函数体/条件分支体/循环体/数组构造
    DotDotDot, // `...` 可变参数
}

impl Display for Token {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Token::Illegal => write!(f, "<illegal>"),
            Token::Eof => write!(f, "<eof>"),
            Token::PrimitiveType(type_) => write!(f, "<primitive-type, {}>", type_),
            Token::IntLit(lit) => write!(f, "<int-lit, {}>", lit),
            Token::FloatLit(lit) => write!(f, "<float-lit, {}>", lit),
            Token::StrLit(lit) => write!(f, "<str-lit, \"{}\">", lit.escape_default()),
            Token::BoolLit(lit) => write!(f, "<bool-lit, {}>", lit),
            Token::CharLit(lit) => write!(f, "<char-lit, '{}'>", (*lit as char).escape_default()),
            Token::Null => write!(f, "<null>"),
            Token::If => write!(f, "<if>"),
            Token::Else => write!(f, "<else>"),
            Token::While => write!(f, "<while>"),
            Token::Break => write!(f, "<break>"),
            Token::Continue => write!(f, "<continue>"),
            Token::Return => write!(f, "<return>"),
            Token::Extern => write!(f, "<extern>"),
            Token::Const => write!(f, "<const>"),
            Token::Ident(ident) => write!(f, "<ident, {}>", ident),
            Token::Equal => write!(f, "<`=`>"),
            Token::Plus => write!(f, "<`+`>"),
            Token::Minus => write!(f, "<`-`>"),
            Token::Star => write!(f, "<`*`>"),
            Token::Slash => write!(f, "<`/`>"),
            Token::Percent => write!(f, "<`%`>"),
            Token::AmpersandAmpersand => write!(f, "<`&&`>"),
            Token::PipePipe => write!(f, "<`||`>"),
            Token::Bang => write!(f, "<`!`>"),
            Token::EqualEqual => write!(f, "<`==`>"),
            Token::BangEqual => write!(f, "<`!=`>"),
            Token::Less => write!(f, "<`<`>"),
            Token::Greater => write!(f, "<`>`>"),
            Token::LessEqual => write!(f, "<`<=`>"),
            Token::GreaterEqual => write!(f, "<`>=`>"),
            Token::Tilde => write!(f, "<`~`>"),
            Token::Caret => write!(f, "<`^`>"),
            Token::Ampersand => write!(f, "<`&`>"),
            Token::Pipe => write!(f, "<`|`>"),
            Token::Comma => write!(f, "<`,`>"),
            Token::SemiColon => write!(f, "<`;`>"),
            Token::LParen => write!(f, "<`(`>"),
            Token::RParen => write!(f, "<`)`>"),
            Token::LBrack => write!(f, "<`[`>"),
            Token::RBrack => write!(f, "<`]`>"),
            Token::LBrace => write!(f, "<`{{`>"),
            Token::RBrace => write!(f, "<`}}`>"),
            Token::DotDotDot => write!(f, "<`...`>"),
        }
    }
}

static RESERVED: phf::Map<&'static str, Token> = phf::phf_map! {
    "null" => Token::Null,
    "if" => Token::If,
    "else" => Token::Else,
    "while" => Token::While,
    "break" => Token::Break,
    "continue" => Token::Continue,
    "return" => Token::Return,
    "extern" => Token::Extern,
    "const" => Token::Const,
    "int" => Token::PrimitiveType(PrimitiveType::Int),
    "float" => Token::PrimitiveType(PrimitiveType::Float),
    "bool" => Token::PrimitiveType(PrimitiveType::Bool),
    "char" => Token::PrimitiveType(PrimitiveType::Char),
    "void" => Token::PrimitiveType(PrimitiveType::Void),
    "true" => Token::BoolLit(true),
    "false" => Token::BoolLit(false),
};

pub fn ident_or_reserved(ident: &str) -> Token {
    RESERVED
        .get(ident)
        .cloned()
        .unwrap_or(Token::Ident(ident.to_owned()))
}
