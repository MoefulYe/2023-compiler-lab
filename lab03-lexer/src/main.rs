use std::{
    env,
    fs::File,
    io::{read_to_string, Result},
    path::PathBuf,
};

use lex::lex;
pub mod error;
pub mod lex;
pub mod span;
pub mod tokens;

extern crate nom;
extern crate nom_locate;

fn main() -> Result<()> {
    for path in env::args().skip(1).map(|s| PathBuf::from(s)) {
        let file_name = path.file_name().unwrap().to_str().unwrap();
        println!("tokenize file: `{file_name}`");
        let code = read_to_string(File::open(&path)?)?;
        lex(file_name, &code);
        println!("-----------------done-----------------");
    }
    Ok(())
}
