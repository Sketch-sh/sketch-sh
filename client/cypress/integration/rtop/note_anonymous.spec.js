const faker = require("faker");

let baseUrl = "http://localhost:3000/";
let url = path => baseUrl + path;

context("note - anonymous user", () => {
  beforeEach(() => {
    cy.visit(url("new"));
  });

  it("create new note and edit it anonymously", () => {
    let title = faker.lorem.words();

    cy.get(".Topbar__action")
      .contains("Save")
      .as("save");

    cy.get("@save").should("be.disabled");

    cy.get(".metadata")
      .find("input")
      .first()
      .as("title")
      .type(title);

    cy.get("@save").should("be.enabled");

    cy.get("textarea")
      .first()
      .type("let a: string = 1;", { force: true });

    cy.get("@save")
      .click()
      .contains("Saving");

    cy.url().should("match", /s\/.+\/.+---.+/, "should not be new route");

    cy.reload(true);

    cy.get("@title").should("have.value", title);

    cy.window().then(win => {
      expect(win.editor.getValue()).to.equal("let a: string = 1;");
    });

    /* Update note anonymously */
    cy.get("@save").should("be.disabled");

    cy.window().then(win => {
      win.editor.setValue("let a = 1;");
    });

    cy.url().then(firstUrl => {
      cy.get("@save")
        .should("be.enabled")
        .click();

      cy.window().then(win => {
        expect(win.editor.getValue()).to.equal("let a = 1;");
      });

      let noteId = firstUrl.split("/")[4];
      
      cy.url().should("contains", noteId, "same note id");
    });
  });
});
