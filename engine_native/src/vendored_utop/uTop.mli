(*
 * uTop.ml
 * -------
 * Copyright : (c) 2011, Jeremie Dimino <jeremie@dimino.org>
 * Licence   : BSD3
 *
 * This file is a part of utop.
 *)
val keywords : Set.Make(String).t ref
(** The set of OCaml keywords. *)

(** Syntax. *)
type syntax =
  | Normal
      (** No camlp4. *)
  | Camlp4o
      (** Camlp4, original syntax. *)
  | Camlp4r
      (** Camlp4, revised syntax. *)
