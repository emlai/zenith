import * as nbind from "nbind";
import * as zenith from "./zenith";

const { App } = nbind.init<typeof zenith>().lib;

App.run();
