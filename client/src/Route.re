/*
   Route.re
   This module contains type definition for all routes in the project
 */
type noteRouteConfig = {
  user: string,
  noteId: string,
  slug: option(string),
};

type t =
  | Home
  | Note(noteRouteConfig)
  | AuthGithub
  | AuthFailure
  | AuthCallback(string)
  | NotFound;

type route = t;

let urlToRoute: ReasonReact.Router.url => t =
  url =>
    switch (url.path) {
    | [""]
    | []
    | ["/"] => Home
    | ["auth", "github"] => AuthGithub
    | ["auth", "failure"] => AuthFailure
    | ["auth", "callback"] =>
      /*
       NOTE: Move querystring parsing outside if there are more
       branches use it
       */
      let parsedQueryString = url.search |. URLSearchParams.make;

      switch (parsedQueryString |. URLSearchParams.get("token")) {
      | Some(token) => AuthCallback(token)
      | None => NotFound
      };
    | [user, noteId] => Note({user, noteId, slug: None})
    | [user, noteId, slug] => Note({user, noteId, slug: Some(slug)})
    | _ => NotFound
    };
