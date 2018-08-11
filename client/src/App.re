Modules.require("./App.css");
open Utils;
open Route;

type state = route;

type action =
  | ChangeView(route);

let component = ReasonReact.reducerComponent("Rtop");

let make = _children => {
  ...component,
  initialState: () =>
    ReasonReact.Router.dangerouslyGetInitialUrl()->Route.urlToRoute,
  reducer: (action, _state) =>
    switch (action) {
    | ChangeView(view) => ReasonReact.Update(view)
    },
  didMount: ({send, onUnmount}) => {
    let watcherID =
      ReasonReact.Router.watchUrl(url =>
        Route.urlToRoute(url)->ChangeView->send
      );
    onUnmount(() => ReasonReact.Router.unwatchUrl(watcherID));
  },
  render: ({state}) =>
    <ReasonApollo.Provider client=GqlClient.instance>
      <UI_Notification.Provider />
      <AuthStatus.Provider />
      (
        switch (state) {
        | Home =>
          <Layout_WithTopbar>
            ...<HomeLazy
                 fetch=(
                   () => [%bs.raw
                     {| import(/* webpackChunkName: "Home" */ "./Home.bs.js") |}
                   ]
                 )
                 onLoading=(() => <UI_FullpageLoading />)
                 render=(((module Home)) => <Home />)
               />
          </Layout_WithTopbar>
        | Note(noteInfo) =>
          <Layout_WithTopbar>
            ...<NoteLazy
                 fetch=(
                   () => [%bs.raw
                     {| import(/* webpackChunkName: "Note" */ "./Note.bs.js") |}
                   ]
                 )
                 onLoading=(() => <UI_FullpageLoading />)
                 render=(((module Note)) => <Note noteInfo />)
               />
          </Layout_WithTopbar>
        | NoteNew =>
          <Layout_WithTopbar>
            ...<NoteNewLazy
                 fetch=(
                   () => [%bs.raw
                     {| import(/* webpackChunkName: "NoteNew" */ "./NoteNew.bs.js") |}
                   ]
                 )
                 onLoading=(() => <UI_FullpageLoading />)
                 render=(((module NoteNew)) => <NoteNew />)
               />
          </Layout_WithTopbar>
        | AuthCallback(token) => <Auth.AuthCallback token />
        | AuthLogout => <Auth.AuthLogout />
        | AuthGithub => <Auth.AuthGithub />
        | AuthFailure => "auth failure"->str
        | EditorDevelopment =>
          Utils.env == "production" ?
            <NotFound /> :
            <Layout_WithTopbar>
              ...<Editor_Note_LoaderLazy
                   fetch=(
                     () => [%bs.raw
                       {| import(/* webpackChunkName: "Editor_Note_Loader" */ "../src_editor/Editor_Note_Loader.bs.js") |}
                     ]
                   )
                   onLoading=(() => <UI_FullpageLoading />)
                   render=(
                     ((module Editor_Note_Loader)) => <Editor_Note_Loader />
                   )
                 />
            </Layout_WithTopbar>
        | NotFound => <NotFound />
        }
      )
      <div className="warning-staging-server">
        "WARNING: This is a staging server of ReasonML playground. All data will be reset every day"
        ->Utils.str
      </div>
    </ReasonApollo.Provider>,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
