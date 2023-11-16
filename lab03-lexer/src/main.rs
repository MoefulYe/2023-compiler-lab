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
        for token in lexer(&code).map(|v| v) {
            println!("{}", token);
        }
        println!("------------done------------");
    }
    Ok(())
}
