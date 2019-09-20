[@bs.deriving abstract]
type unsafe_t = {
  hash: string,
  host: string,
  hostname: string,
  href: string,
  origin: string,
  password: string,
  pathname: string,
  port: string,
  protocol: string,
  search: string,
  searchParams: string,
  username: string,
};

[@bs.new] [@bs.module "url"] external make_unsafe: string => unsafe_t = "URL";

type t = {
  hash: option(string),
  host: option(string),
  hostname: option(string),
  href: option(string),
  origin: option(string),
  password: option(string),
  pathname: option(string),
  port: option(string),
  protocol: option(string),
  search: option(string),
  searchParams: option(string),
  username: option(string),
};

let make = string =>
  try (
    {
      let url = make_unsafe(string);
      Some({
        hash: url->hashGet->String.toOption,
        host: url->hostGet->String.toOption,
        hostname: url->hostnameGet->String.toOption,
        href: url->hrefGet->String.toOption,
        origin: url->originGet->String.toOption,
        password: url->passwordGet->String.toOption,
        pathname: url->pathnameGet->String.toOption,
        port: url->portGet->String.toOption,
        protocol: url->protocolGet->String.toOption,
        search: url->searchGet->String.toOption,
        searchParams: url->searchParamsGet->String.toOption,
        username: url->usernameGet->String.toOption,
      });
    }
  ) {
  | _exn => None
  };
