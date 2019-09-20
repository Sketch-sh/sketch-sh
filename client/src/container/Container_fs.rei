/**
 * Naive file system that's backed by a mutable map
 */

/** Add/Update a file */
let set: (~filename: string, ~content: string) => unit;

/** Get a file. return option(string) to indicate whethere the file exists */
let get: (~filename: string) => option(string);

/** Delete a file */
let delete: (~filename: string) => unit;
