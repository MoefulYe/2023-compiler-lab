use std::{
    env,
    fs::File,
    io::{read_to_string, Result},
};

use lex::lexer;
pub mod lex;
pub mod tokens;

extern crate nom;
extern crate nom_locate;

fn main() -> Result<()> {
    for file in env::args().skip(1) {
        println!("tokenize file: `{file}`");
        let code = read_to_string(File::open(file)?)?;
        let mut it = lexer(&code);
        it.for_each(|token| println!("{}", token));
        let res = it.finish();
        assert!(res.is_ok());
        println!("------------done------------");
    }
    Ok(())
}
