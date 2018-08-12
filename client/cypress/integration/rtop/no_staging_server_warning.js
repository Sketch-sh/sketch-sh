const faker = require("faker");

let baseUrl = "http://localhost:3000/";
let url = path => baseUrl + path;

context("no staging server warning", () => {
  it("don't show warning on localhost", () => {
    cy.visit(url(""));
    cy.get(".warning-staging-server").should("not.exist");
  });

  it("show warning on staging host", () => {
    cy.visit("https://rtop.khoa.xyz");
    cy.get(".warning-staging-server").should("be.visible");
  });
});
