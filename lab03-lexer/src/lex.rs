use crate::tokens::{ident_or_reserved, Token};
use nom::branch::alt;
use nom::bytes::complete::{tag, tag_no_case, take_until};
use nom::character::complete::{alpha1, alphanumeric1, char, digit1, i32, multispace0, one_of};
use nom::combinator::{iterator, map, map_res, opt, recognize, ParserIterator};
use nom::multi::many0_count;
use nom::sequence::{delimited, pair, tuple};
use nom::IResult;
use nom_locate::LocatedSpan;

pub type Span<'a> = LocatedSpan<&'a str>;
pub type LexResult<'a, T> = IResult<Span<'a>, T>;

// impl Iterator for Parser<'_> {
//
//     fn next(&mut self) -> Option<Self::Item> {
//         todo!()
//     }
// }

macro_rules! syntax {
    ($func_name: ident, $tag_string: literal, $output_token: expr) => {
        fn $func_name(input: Span) -> LexResult<Token> {
            map(tag($tag_string), |_| $output_token)(input)
        }
    };
}

pub fn lex_token(input: Span) -> LexResult<Token> {
    delimited(
        skip,
        alt((
            lex_int_lit,
            lex_float_lit,
            lex_str_lit,
            lex_char_lit,
            lex_operators,
            lex_punctuator,
            lex_ident_or_reserved,
            lex_illegal,
        )),
        skip,
    )(input)
}

fn skip(input: Span) -> LexResult<()> {
    let multi_line_comment = recognize(tuple((
        multispace0,
        tag("/*"),
        take_until("*/"),
        tag("*/"),
        multispace0,
    )));
    let one_line_comment = recognize(tuple((
        multispace0,
        tag("//"),
        take_until("\n"),
        tag("\n"),
        multispace0,
    )));

    return map(
        alt((one_line_comment, multi_line_comment, multispace0)),
        |_| (),
    )(input);
}

syntax!(lex_equal, "=", Token::Equal);
syntax!(lex_plus, "+", Token::Plus);
syntax!(lex_minus, "-", Token::Minus);
syntax!(lex_star, "*", Token::Star);
syntax!(lex_slash, "/", Token::Slash);
syntax!(lex_percent, "%", Token::Percent);
syntax!(lex_ampersand_ampersand, "&&", Token::AmpersandAmpersand);
syntax!(lex_pipe_pipe, "||", Token::PipePipe);
syntax!(lex_bang, "!", Token::Bang);
syntax!(lex_equal_equal, "==", Token::EqualEqual);
syntax!(lex_bang_equal, "!=", Token::BangEqual);
syntax!(lex_less, "<", Token::Less);
syntax!(lex_greater, ">", Token::Greater);
syntax!(lex_less_equal, "<=", Token::LessEqual);
syntax!(lex_greater_equal, ">=", Token::GreaterEqual);
syntax!(lex_tilde, "~", Token::Tilde);
syntax!(lex_caret, "^", Token::Caret);
syntax!(lex_ampersand, "&", Token::Ampersand);
syntax!(lex_pipe, "|", Token::Pipe);
syntax!(lex_comma, ",", Token::Comma);
syntax!(lex_semi_colon, ";", Token::SemiColon);
syntax!(lex_lparen, "(", Token::LParen);
syntax!(lex_rparen, ")", Token::RParen);
syntax!(lex_lbrack, "[", Token::LBrack);
syntax!(lex_rbrack, "]", Token::RBrack);
syntax!(lex_lbrace, "{", Token::LBrace);
syntax!(lex_rbrace, "}", Token::RBrace);
syntax!(lex_dot_dot_dot, "...", Token::DotDotDot);

fn lex_operators(input: Span) -> LexResult<Token> {
    alt((
        lex_equal_equal,
        lex_equal,
        lex_plus,
        lex_minus,
        lex_star,
        lex_slash,
        lex_percent,
        lex_ampersand_ampersand,
        lex_pipe_pipe,
        lex_bang_equal,
        lex_bang,
        lex_less_equal,
        lex_less,
        lex_greater_equal,
        lex_greater,
        lex_tilde,
        lex_caret,
        lex_ampersand,
        lex_pipe,
    ))(input)
}

fn lex_punctuator(input: Span) -> LexResult<Token> {
    alt((
        lex_comma,
        lex_semi_colon,
        lex_lparen,
        lex_rparen,
        lex_lbrack,
        lex_rbrack,
        lex_lbrace,
        lex_rbrace,
        lex_dot_dot_dot,
    ))(input)
}

fn lex_illegal(input: Span) -> LexResult<Token> {
    map(nom::bytes::complete::take(0usize), |_| Token::Eof)(input)
}

fn lex_ident_or_reserved(input: Span) -> LexResult<Token> {
    let (leftover, ident) = recognize(pair(
        alt((tag("_"), alpha1)),
        many0_count(alt((tag("_"), alphanumeric1))),
    ))(input)?;
    return Ok((leftover, ident_or_reserved(ident.fragment())));
}

use lex_char::lex as lex_char_lit;
use lex_float::lex as lex_float_lit;
use lex_int::lex as lex_int_lit;
use lex_str::lex as lex_str_lit;

mod lex_str {
    use nom::{
        branch::alt,
        bytes::complete::is_not,
        character::complete::char,
        combinator::{map, value, verify},
        multi::fold_many0,
        sequence::{delimited, preceded},
        Parser,
    };

    use super::{LexResult, Span, Token};

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum StringFragment<'a> {
        Literal(&'a str),
        EscapedChar(char),
    }

    fn lex_escaped(input: Span) -> LexResult<char> {
        preceded(
            char('\\'),
            alt((
                value('\t', char('t')),
                value('\r', char('r')),
                value('\n', char('n')),
                value('"', char('"')),
                value('\\', char('\\')),
            )),
        )(input)
    }

    fn lex_lit<'a>(input: Span<'a>) -> LexResult<&'a str> {
        let not_quote_slash = is_not("\"\\");
        let (leftover, parsed) =
            verify(not_quote_slash, |s: &Span| !s.fragment().is_empty()).parse(input)?;
        Ok((leftover, parsed.fragment()))
    }

    fn lex_fragment(input: Span) -> LexResult<StringFragment> {
        alt((
            map(lex_escaped, StringFragment::EscapedChar),
            map(lex_lit, StringFragment::Literal),
        ))(input)
    }

    pub fn lex(input: Span) -> LexResult<Token> {
        let parse_inner = fold_many0(lex_fragment, String::new, |mut acc, fragment| {
            match fragment {
                StringFragment::Literal(s) => acc.push_str(s),
                StringFragment::EscapedChar(c) => acc.push(c),
            }
            acc
        });
        map(delimited(char('"'), parse_inner, char('"')), Token::StrLit)(input)
    }

    #[cfg(test)]
    mod test {
        use crate::tokens::Token;

        use super::lex;
        use super::Span;
        #[test]
        fn test_str() {
            assert_eq!(
                lex(Span::new(r###""hello world""###)).unwrap().1,
                Token::StrLit("hello world".to_owned())
            );
            assert_eq!(
                lex(Span::new(r###""hello\nworld""###)).unwrap().1,
                Token::StrLit("hello\nworld".to_owned())
            );
            assert_eq!(
                lex(Span::new(r###""hello\tworld""###)).unwrap().1,
                Token::StrLit("hello\tworld".to_owned())
            );
        }
    }
}

mod lex_char {
    use nom::branch::alt;
    use nom::character::complete::char;
    use nom::combinator::map;
    use nom::combinator::value;
    use nom::sequence::delimited;
    use nom::sequence::preceded;
    use nom::Parser;

    use crate::tokens::Token;

    use super::LexResult;
    use super::Span;
    fn lex_escaped(input: Span) -> LexResult<u8> {
        preceded(
            char('\\'),
            alt((
                value(b'\t', char('t')),
                value(b'\r', char('r')),
                value(b'\n', char('n')),
                value(b'\'', char('\'')),
                value(b'\\', char('\\')),
            )),
        )(input)
    }

    fn lex_normal(input: Span) -> LexResult<u8> {
        let (leftover, parsed) = nom::bytes::complete::take(1usize).parse(input)?;
        let ch = parsed.fragment().chars().next().unwrap();
        assert!(ch.is_ascii(), "char literal must be ascii");
        Ok((leftover, ch as u8))
    }

    pub fn lex(input: Span) -> LexResult<Token> {
        map(
            delimited(char('\''), alt((lex_escaped, lex_normal)), char('\'')),
            Token::CharLit,
        )(input)
    }

    #[cfg(test)]
    mod test {
        use crate::tokens::Token;

        use super::lex;
        use super::Span;
        #[test]
        fn test_lex() {
            assert_eq!(lex(Span::new("'a'")).unwrap().1, Token::CharLit(b'a'));
            assert_eq!(lex(Span::new("'\\n'")).unwrap().1, Token::CharLit(b'\n'));
            assert_eq!(lex(Span::new("'\\t'")).unwrap().1, Token::CharLit(b'\t'));
            assert_eq!(lex(Span::new("'\\r'")).unwrap().1, Token::CharLit(b'\r'));
            assert_eq!(lex(Span::new("'\\''")).unwrap().1, Token::CharLit(b'\''));
            assert_eq!(lex(Span::new("'\\\\'")).unwrap().1, Token::CharLit(b'\\'));
        }
    }
}

mod lex_float {
    use super::*;
    fn has_interger_part(input: Span) -> LexResult<Span> {
        type Error<'a> = nom::error::Error<Span<'a>>;
        let integer_part = pair(opt(one_of::<_, _, Error>("+-")), digit1);
        let fractional_part = pair(char::<_, Error>('.'), digit1);
        let exponent_part = pair(
            tag_no_case("e"),
            pair(opt(one_of::<_, _, Error>("+-")), digit1),
        );
        recognize(tuple((
            integer_part,
            opt(fractional_part),
            opt(exponent_part),
        )))(input)
    }

    fn no_interger_part(input: Span) -> LexResult<Span> {
        type Error<'a> = nom::error::Error<Span<'a>>;
        let fractional_part = pair(char::<_, Error>('.'), digit1);
        let exponent_part = pair(
            tag_no_case("e"),
            pair(opt(one_of::<_, _, Error>("+-")), digit1),
        );
        recognize(tuple((fractional_part, opt(exponent_part))))(input)
    }

    pub fn lex(input: Span) -> LexResult<Token> {
        map(
            map_res(alt((has_interger_part, no_interger_part)), |s: Span| {
                s.fragment().parse::<f32>()
            }),
            Token::FloatLit,
        )(input)
    }

    #[cfg(test)]
    mod test {
        use super::lex;
        use super::Span;
        use super::Token;
        #[test]
        fn test_float() {
            assert_eq!(lex(Span::new("1.0")).unwrap().1, Token::FloatLit(1.0));
            assert_eq!(lex(Span::new("1.0e1")).unwrap().1, Token::FloatLit(10.0));
            assert_eq!(lex(Span::new("1.0e-1")).unwrap().1, Token::FloatLit(0.1));
            assert_eq!(lex(Span::new("1.0e+1")).unwrap().1, Token::FloatLit(10.0));
            assert_eq!(lex(Span::new("1.0E1")).unwrap().1, Token::FloatLit(10.0));
            assert_eq!(lex(Span::new("1.0E-1")).unwrap().1, Token::FloatLit(0.1));
            assert_eq!(lex(Span::new(".1E+1")).unwrap().1, Token::FloatLit(1.0));
        }
    }
}

mod lex_int {
    use super::*;
    use nom::{
        character::complete::{hex_digit1, oct_digit1},
        sequence::preceded,
    };

    fn hex(input: Span) -> LexResult<i32> {
        map_res(preceded(tag_no_case("0x"), hex_digit1), |s: Span| {
            i32::from_str_radix(s.fragment(), 16)
        })(input)
    }

    fn bin(input: Span) -> LexResult<i32> {
        type Error<'a> = nom::error::Error<Span<'a>>;
        map_res(
            preceded(
                tag_no_case("0b"),
                recognize(many0_count(one_of::<_, _, Error>("01"))),
            ),
            |s: Span| i32::from_str_radix(s.fragment(), 2),
        )(input)
    }

    fn oct(input: Span) -> LexResult<i32> {
        type Error<'a> = nom::error::Error<Span<'a>>;
        map_res(preceded(tag_no_case("0o"), oct_digit1), |s: Span| {
            i32::from_str_radix(s.fragment(), 8)
        })(input)
    }

    fn dec(input: Span) -> LexResult<i32> {
        i32(input)
    }

    pub fn lex(input: Span) -> LexResult<Token> {
        map(alt((hex, bin, oct, dec)), Token::IntLit)(input)
    }

    #[cfg(test)]
    mod test {
        use super::super::Span;
        use super::lex;
        use super::Token;

        #[test]
        fn test_interger() {
            assert_eq!(lex(Span::new("0x1")).unwrap().1, Token::IntLit(1));
            assert_eq!(lex(Span::new("0b1")).unwrap().1, Token::IntLit(1));
            assert_eq!(lex(Span::new("0o1")).unwrap().1, Token::IntLit(1));
            assert_eq!(lex(Span::new("1")).unwrap().1, Token::IntLit(1));
        }
    }
}
