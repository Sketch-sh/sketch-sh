Modules.require("./App.css");
open Utils;

let homeImport = () => [%bs.raw
  {| import(/* webpackChunkName: "Home" */ "./Home.bs.js") |}
];

let noteImport = () => [%bs.raw
  {| import(/* webpackChunkName: "Note" */ "./Note.bs.js") |}
];

let noteTemplateImport = () => [%bs.raw
  {| import(/* webpackChunkName: "NoteTemplateChoose" */ "./NoteTemplateChoose.bs.js") |}
];

let userImport = () => [%bs.raw
  {| import(/* webpackChunkName: "User" */ "./User.bs.js") |}
];

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();
  let route = url->Route.urlToRoute;

  <ReasonApollo.Provider client=GqlClient.instance>
    <Helmet defaultTitle="Sketch.sh" titleTemplate="%s - Sketch.sh" />
    <ButterToast.Tray />
    <Router.Unload.Provider />
    <AuthStatus.Provider />
    <FontFaceObserver.Provider />
    {switch (route) {
     | Home =>
       <Layout_WithTopbar>
         ...<AuthStatus.IsAuthenticated>
              ...{authState =>
                switch (authState) {
                | Anonymous =>
                  let (lang, links, blocks) = Editor_Introduction.blocks;
                  <NoteNewLazy
                    title="Sketch.sh - Interactive ReasonML sketchbook"
                    blocks
                    links
                    lang
                  />;
                | Login(userId) =>
                  <HomeLazy
                    fetch=homeImport
                    onLoading={() => <UI_FullpageLoading />}
                    render={((module Home)) => <Home userId />}
                  />
                }
              }
            </AuthStatus.IsAuthenticated>
       </Layout_WithTopbar>
     | Note(noteInfo) =>
       <Layout_WithTopbar>
         ...<NoteLazy
              fetch=noteImport
              onLoading={() => <UI_FullpageLoading />}
              render={((module Note)) => <Note noteInfo />}
            />
       </Layout_WithTopbar>
     | NoteTemplateChoose =>
       <Layout_WithTopbar>
         ...<NoteTemplateChooseLazy
              fetch=noteTemplateImport
              onLoading={() => <UI_FullpageLoading />}
              render={((module NoteTemplateChoose)) => <NoteTemplateChoose />}
            />
       </Layout_WithTopbar>
     | NoteNew(lang) =>
       <Layout_WithTopbar> ...<NoteNewLazy lang /> </Layout_WithTopbar>
     | User(userName) =>
       <Layout_WithTopbar>
         ...<UserLazy
              fetch=userImport
              onLoading={() => <UI_FullpageLoading />}
              render={((module User)) => <User userName />}
            />
       </Layout_WithTopbar>
     | AuthCallback(token) => <Auth.AuthCallback token />
     | AuthLogout => <Logout />
     | AuthGithub => <Auth.AuthGithub />
     | AuthFailure => "auth failure"->str
     | NotFound => <NotFound />
     }}
  </ReasonApollo.Provider>;
};
