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
    cy.visit("new/reason");
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

  it("ctrl+enter should execute without creating new block", () => {
    assertBlocks(1);

    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a: string = 1;", { force: true });
    assertErrorsOrWarnings(0);

    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(1);

    cy.get("@block1")
      .clear({ force: true })
      .type("let a = 1;", { force: true });
    assertErrorsOrWarnings(0);

    shortcut("{ctrl}{enter}");

    assertErrorsOrWarnings(0);
    cy.get("@block1")
      .get(".widget__value")
      .should("contain", "let a: int = 1;");
  });
});

context("language toggle", () => {
  it("should have language switcher", () => {
    cy.visit("new/reason");

    cy.get(`fieldset[aria-label="Language toggle"]`).should("be.visible");
  });

  it("should be default to ReasonML", () => {
    cy.visit("new/reason");
    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=RE]")
      .should("be.checked");
  });

  it("should persist language to database - ReasonML", () => {
    cy.visit("new/reason");

    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=RE]")
      .should("be.checked");

    cy.get(".block__container")
      .first()
      .as("block1")
      .find("textarea")
      .as("code1")
      .type(
        `let value = "awesome";
      switch (value) {
      | "awesome" => true
      | _ => false
      };`,
        { force: true }
      );
    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(0);
    cy.get("@block1")
      .find(".widget__value")
      .eq(1)
      .should("have.text", "- : bool = true\n");

    shortcut("{ctrl}s");
    cy.url().should("match", /s\/.+/, "should not be new route");
    cy.reload();
    cy.get("@block1")
      .find(".widget__value")
      .eq(1)
      .should("have.text", "- : bool = true\n");
  });

  it("should persist language to database - OCaml", () => {
    cy.visit("new/reason");

    cy.get("input[id=ML]").check({ force: true });

    cy.get(".block__container")
      .first()
      .as("block1")
      .find("textarea")
      .as("code1")
      .type(
        `let value = "awesome" in
      match value with
      | "awesome" -> true
      | _ -> false`,
        { force: true }
      );
    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(0);
    cy.get("@block1")
      .find(".widget__value")
      .eq(0)
      .should("have.text", "- : bool = true\n");

    shortcut("{ctrl}s");
    cy.url().should("match", /s\/.+/, "should not be new route");
    cy.reload();
    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=ML]")
      .should("be.checked");
    cy.get("@block1")
      .find(".widget__value")
      .eq(0)
      .should("have.text", "- : bool = true\n");
  });

  it("should execute the code when language changed", () => {
    cy.visit("new/reason");

    cy.get(`fieldset[aria-label="Language toggle"]`)
      .get("input[id=RE]")
      .should("be.checked");

    cy.get(".block__container")
      .first()
      .as("block1")
      .find("textarea")
      .as("code1")
      .type(
        `let value = "awesome";
      switch (value) {
      | "awesome" => true
      | _ => false
      };`,
        { force: true }
      );
    shortcut("{ctrl}{enter}");
    assertErrorsOrWarnings(0);
    cy.get("input[id=ML]").check({ force: true });
    assertErrorsOrWarnings(1);
  });
});

context("Edge cases", () => {
  it("should not prevent editing title after focusing on a block #105", () => {
    // https://github.com/Sketch-sh/sketch-sh/issues/105

    cy.visit("new/reason");

    cy.get(".EditorNote__metadata")
      .find("input")
      .first()
      .as("title")
      .type(faker.lorem.words());

    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a = 1;", { force: true });

    cy.get("@title")
      .focus()
      .clear()
      .type("new title");

    cy.get("@title")
      .should("have.value", "new title")
      .focused();
  });

  it("should display value in correct order when there are multiple expressions on the same line", () => {
    cy.visit("new/reason");
    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let x = 1; let y = 2; let z = 3;", { force: true });
    shortcut("{ctrl}{enter}");
    assertValue(3);

    cy.get(".widget__value").as("value");
    cy.get("@value")
      .eq(0)
      .should("have.text", "let x: int = 1;\n");
    cy.get("@value")
      .eq(1)
      .should("have.text", "let y: int = 2;\n");
    cy.get("@value")
      .eq(2)
      .should("have.text", "let z: int = 3;\n");
  });
});
