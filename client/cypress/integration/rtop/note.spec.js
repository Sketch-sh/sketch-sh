/// <reference types="Cypress" />

const faker = require("faker");

/* 
 * Helpers
 */
const assertBlocks = howManyBlocks =>
  cy.get(".block__container").should("have.length", howManyBlocks);
const assertErrorsOrWarnings = howMany =>
  cy.get(".widget__lint").should("have.length", howMany);
const assertValue = howMany =>
  cy.get(".widget__value").should("have.length", howMany);
const shortcut = shortcut =>
  cy
    .get("body")
    .focus()
    .type(shortcut);

context("keyboard shortcuts", () => {
  beforeEach(() => {
    cy.visit("new");
  });
  it("ctrl+s for save", () => {
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

    shortcut("{ctrl}s");
    cy.get("@save").should("be.disabled");

    // Repeat
    cy.get("@title").type(faker.lorem.words());
    cy.get("@save").should("be.enabled");

    shortcut("{ctrl}s");

    cy.get("@save").should("be.disabled");
  });

  it("ctrl+enter should execute without creating new block", () => {
    assertBlocks(1);

    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a: string = 1;", { force: true });
    assertErrorsOrWarnings(0);

    cy.get("body")
      .focus()
      .type("{ctrl}{enter}");
    assertErrorsOrWarnings(1);

    cy.get("@block1")
      .clear({ force: true })
      .type("let a = 1;", { force: true });
    assertErrorsOrWarnings(0);

    shortcut("{ctrl}{enter}");

    cy.get(".widget__lint").should("have.length", 0);
    cy.get("@block1")
      .get(".widget__value")
      .should("contain", "let a: int = 1;");
  });

  it("shift+enter execute and focus next block/or create new block", () => {
    assertBlocks(1);
    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a = 1;", { force: true });

    shortcut("{shift}{enter}");
    assertValue(1);
    assertBlocks(2);

    cy.get(".block__container")
      .eq(1)
      .find("textarea")
      .as("block2")
      .type("print_int(a);", { force: true });

    shortcut("{shift}{enter}");
    assertValue(2);
    assertBlocks(3);

    shortcut("{shift}{enter}");
    shortcut("{shift}{enter}");
    assertBlocks(5);

    cy.get(".block__container")
      .eq(4)
      .find("textarea")
      .focused();

    cy.get(".block__container")
      .eq(2)
      .find("textarea")
      .focus();

    shortcut("{shift}{enter}");
    cy.get(".block__container")
      .eq(3)
      .find("textarea")
      .focused();
    assertBlocks(5);

    shortcut("{shift}{enter}");
    cy.get(".block__container")
      .eq(4)
      .find("textarea")
      .focused();
    assertBlocks(5);
  });
});
