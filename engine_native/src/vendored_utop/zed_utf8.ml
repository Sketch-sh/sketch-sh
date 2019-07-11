(*
 * zed_utf8.ml
 * -----------
 * Copyright : (c) 2011, Jeremie Dimino <jeremie@dimino.org>
 * Licence   : BSD3
 *
 * This file is a part of Zed, an editor engine.
 *)
type t = string

exception Invalid of string * string

let fail str pos msg = raise (Invalid(Printf.sprintf "at position %d: %s" pos msg, str))

let rec unsafe_sub_equal str ofs sub ofs_sub =
  if ofs_sub = String.length sub then
    true
  else
    (String.unsafe_get str ofs = String.unsafe_get sub ofs_sub)
    && unsafe_sub_equal str (ofs + 1) sub (ofs_sub + 1)

let starts_with str prefix =
  if String.length prefix > String.length str then
    false
  else
    unsafe_sub_equal str 0 prefix 0

let ends_with str suffix =
  let ofs = String.length str - String.length suffix in
  if ofs < 0 then
    false
  else
    unsafe_sub_equal str ofs suffix 0

let unsafe_next str ofs =
  match String.unsafe_get str ofs with
    | '\x00' .. '\x7f' ->
        ofs + 1
    | '\xc0' .. '\xdf' ->
        if ofs + 2 > String.length str then
          fail str ofs "unterminated UTF-8 sequence"
        else
          ofs + 2
    | '\xe0' .. '\xef' ->
        if ofs + 3 > String.length str then
          fail str ofs "unterminated UTF-8 sequence"
        else
          ofs + 3
    | '\xf0' .. '\xf7' ->
        if ofs + 4 > String.length str then
          fail str ofs "unterminated UTF-8 sequence"
        else
          ofs + 4
    | _ ->
        fail str ofs "invalid start of UTF-8 sequence"

let rec length_rec str ofs len =
  if ofs = String.length str then
    len
  else
    length_rec str (unsafe_next str ofs) (len + 1)

let length str =
length_rec str 0 0
