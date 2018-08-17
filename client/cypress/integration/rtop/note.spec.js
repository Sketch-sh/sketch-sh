const faker = require("faker");

context("keyboard shortcuts", () => {
  it.only("ctrl+s for save", () => {
    cy.visit("new");

    let title = faker.lorem.words();

    cy.get(".Topbar__action")
      .contains("Save")
      .as("save");

    cy.get("@save").should("be.disabled");

    cy.get(".EditorNote__metadata")
      .find("input")
      .first()
      .as("title")
      .type(title);

    cy.get("@save").should("be.enabled");

    cy.get("textarea")
      .first()
      .type("let a: string = 1;", { force: true });
    cy.get("body").focus().type("{ctrl}s");
    cy.get("@save").should("be.disabled");

    // Repeat
    cy.get("@title").type(faker.lorem.words());
    cy.get("@save").should("be.enabled");
    cy.get("body").focus().type("{ctrl}s");
    cy.get("@save").should("be.disabled");
  });
});
