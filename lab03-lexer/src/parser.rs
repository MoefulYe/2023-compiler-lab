use crate::tokens::{ident_or_reserved, Token};
use nom::branch::alt;
use nom::bytes::complete::tag;
use nom::character::complete::{alpha1, alphanumeric1, i32};
use nom::combinator::{map, recognize};
use nom::multi::many0_count;
use nom::sequence::pair;
use nom::{multi::fold_many0, IResult, Parser};
use nom_locate::LocatedSpan;

pub type Span<'a> = LocatedSpan<&'a str>;
pub type ParseResult<'a, T> = IResult<Span<'a>, T>;

pub struct Lexer<'a> {
    input: Span<'a>,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        Self {
            input: Span::new(input),
        }
    }
}

// impl Iterator for Parser<'_> {
//
//     fn next(&mut self) -> Option<Self::Item> {
//         todo!()
//     }
// }

macro_rules! syntax {
    ($func_name: ident, $tag_string: literal, $output_token: expr) => {
        fn $func_name(input: Span) -> ParseResult<Token> {
            map(tag($tag_string), |_| $output_token)(input)
        }
    };
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

fn lex_ident_or_reserved(input: Span) -> ParseResult<Token> {
    let (leftover, ident) = recognize(pair(
        alt((tag("_"), alpha1)),
        many0_count(alt((tag("_"), alphanumeric1))),
    ))(input)?;
    return Ok((leftover, ident_or_reserved(ident.fragment())));
}

use lex_char::lex as lex_char_lit;
use lex_str::lex as lex_str_lit;

fn lex_int_lit(input: Span) -> ParseResult<Token> {
    map(i32, |num| Token::IntLit(num))(input)
}

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

    use super::{ParseResult, Span};

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    enum StringFragment<'a> {
        Literal(&'a str),
        EscapedChar(char),
    }

    fn lex_escaped(input: Span) -> ParseResult<char> {
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

    fn lex_lit<'a>(input: Span<'a>) -> ParseResult<&'a str> {
        let not_quote_slash = is_not("\"\\");
        let (leftover, parsed) =
            verify(not_quote_slash, |s: &Span| !s.fragment().is_empty()).parse(input)?;
        Ok((leftover, parsed.fragment()))
    }

    fn lex_fragment(input: Span) -> ParseResult<StringFragment> {
        alt((
            map(lex_escaped, StringFragment::EscapedChar),
            map(lex_lit, StringFragment::Literal),
        ))(input)
    }

    pub fn lex(input: Span) -> ParseResult<String> {
        let parse_inner = fold_many0(lex_fragment, String::new, |mut acc, fragment| {
            match fragment {
                StringFragment::Literal(s) => acc.push_str(s),
                StringFragment::EscapedChar(c) => acc.push(c),
            }
            acc
        });
        delimited(char('"'), parse_inner, char('"'))(input)
    }
}

mod lex_char {
    use nom::branch::alt;
    use nom::character::complete::char;
    use nom::combinator::value;
    use nom::sequence::delimited;
    use nom::sequence::preceded;
    use nom::Parser;

    use super::ParseResult;
    use super::Span;
    fn lex_escaped(input: Span) -> ParseResult<u8> {
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

    fn lex_normal(input: Span) -> ParseResult<u8> {
        nom::bytes::complete::take(1usize)
            .parse(input)
            .map(|(leftover, parsed)| (leftover, parsed.fragment().as_bytes()[0]))
    }

    pub fn lex(input: Span) -> ParseResult<u8> {
        delimited(char('\''), alt((lex_escaped, lex_normal)), char('\''))(input)
    }
}
