open Editor_Types.Link;

let getNameFromLink = link =>
  switch (link) {
  | Internal(internalLink) => internalLink.name
  | _ => failwith("There are no external links yet.")
  };

let getIdFromLink = link =>
  switch (link) {
  | Internal(link) => link.sketch_id
  | _ => failwith("There are no external links yet.")
  };
