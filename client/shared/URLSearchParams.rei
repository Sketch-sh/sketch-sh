type t;

let make: string => t;

let get: (t, string) => option(string);

let has: (t, string) => bool;
