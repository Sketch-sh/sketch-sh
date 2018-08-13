const faker = require("faker");

context("note - anonymous user", () => {
  beforeEach(() => {
    cy.visit("new");
  });

  it("create new note and edit it anonymously", () => {
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

    cy.get("@save").click();

    cy.url({ timeout: 10000 }).should(
      "match",
      /s\/.+\/.+---.+/,
      "should go to new url"
    );

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

  it.only("prompt before leaving with dirty state", () => {
    let title = faker.lorem.words();

    cy.get(".Topbar__action")
      .contains("Save")
      .as("save");

    cy.get(".EditorNote__metadata")
      .find("input")
      .first()
      .as("title")
      .type(title);

    cy.get("@save").should("be.enabled");

    const stub = cy.stub();
    stub.onFirstCall().returns(false);

    cy.on("window:confirm", stub);

    cy.get(`a[href="/"]`)
      .click()
      .then(() => {
        expect(stub).to.be.calledOnce;
      });

    cy.location("pathname").should("equal", "/new");

    cy.get("@save").click();
    cy.get("@save").should("be.disabled");

    cy.url().should("contains", "new");
    cy.get(`a[href="/"]`).click();
    cy.url().should("not.contains", "new");
    cy.location("pathname").should("equal", "/");
  });
});
